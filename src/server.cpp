#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>     // used for concurrent clients
#include <fstream>    // for reading and creating files
#include <filesystem> // used for finding the size of a file
#include<vector>      // standard vector library
#include<sstream>     // string stream library for easier string functions
#include <zlib.h>     // used for GZIP compression

using namespace std;

// a function to trim the spaces
string trim(const string& s) {
    string result;
    bool started = false;

    for (int i = 0; i < s.size(); i++) {
        if (!started && s[i] == ' ') {
            continue; // Skip leading spaces
        } else {
            started = true;
            result += s[i];
        }
    }

    // Now remove trailing spaces manually
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }

    return result;
}
// --------------------------------------------------------------------
// Function for Gzip compression

string gzip_compression(const string& input){
  // setting up the z_stream struct and initializing it to 0
  z_stream zs;
  memset(&zs,0,sizeof(zs));

  // deflateInit2 is used to specify gzip format
  // 15 is the default window size and 16 is added to the window size to enable gzip encoding.
  if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {

        throw(std::runtime_error("deflateInit2 failed while compressing."));
  }

  // 2. Set the input data
  zs.next_in = (Bytef*)input.data();
  zs.avail_in = input.size();

  // 3. create and set output buffer
  int ret;
  char outbuffer[32768]; // 32kb buffer
  string compressed_string;

  // 4. main compression loop
  do {
    zs.next_out = (Bytef*)outbuffer;
    zs.avail_out = sizeof(outbuffer);
    ret = deflate(&zs, Z_FINISH);
    if (compressed_string.size() < zs.total_out) {
        compressed_string.append(outbuffer, zs.total_out - compressed_string.size());
    }
  } while (ret == Z_OK);

  // 5. Clean up and free internal zlib resources
  deflateEnd(&zs);

  if (ret != Z_STREAM_END) {
    throw(std::runtime_error("Exception during zlib compression: " + std::to_string(ret)));
  }


  return compressed_string;

}
// --------------------------------------------------------------------
// Function to handle a client
void handle_client(int client_fd,string directory_path){
  if (client_fd > 0){
    while(true){
      cout << "Client connected\n";
      char buffer[1024];
      int bytes_recieved=read(client_fd, buffer, sizeof(buffer)-1);
      if(bytes_recieved<0){
        cerr<<"Failed to read from server\n";
        return ;
      }
      else{
        string request_string(buffer);
        size_t x=request_string.find(" "); // start pos of response
        size_t y=request_string.find(" ",x+1); // end pos of response
        string http_response; // the response i will send
        string request=request_string.substr(x+1,y-(x+1)); // getting the request to give the appropriate response.
        string method = request_string.substr(0, request_string.find(" "));
        if(request=="/"){
          http_response = "HTTP/1.1 200 OK\r\n\r\n";
        }
        else if(request.substr(0, 6)=="/echo/"){
        
          // if request contains the word "echo", then print the word after "echo" command in your response
          
          x=request.find("/",1);
          y=request_string.find(" ",x+1);
          
          string echo_str=request.substr(x+1,y-(x+1));
          http_response="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
          // Checking if the request string has Accept Encoding header
          if(request_string.find("Accept-Encoding:")!=string::npos){
            bool check=false;
            x=request_string.find("Accept-Encoding:");
            x=request_string.find(" ",x+1);
            y=request_string.find("\r\n",x+1);
            string AcceptEncoding=request_string.substr(x+1,y-(x+1));
            stringstream ss(AcceptEncoding);
            string encoding;
            vector<string> encodings;
  
            while (getline(ss, encoding, ',')) {
                encodings.push_back(trim(encoding));
            }
            for(int i=0;i<encodings.size();i++){
              if(encodings[i]=="gzip"){
                check=true;
              }
            }
            if(check){
              string compressed=gzip_compression(echo_str);
              http_response+=to_string(compressed.length());
              http_response+="\r\n";
              http_response+="Content-Encoding: gzip";
              http_response+="\r\n";
              http_response+="\r\n";
              http_response+=compressed;
            }
            else{
              http_response+=to_string(echo_str.length());
              http_response+="\r\n";
              http_response+="\r\n";
              http_response+=echo_str;
            }
          }
          else{
            http_response+=to_string(echo_str.length());
            http_response+="\r\n";
            http_response+="\r\n";
            http_response+=echo_str;
          }
          
        }
        else if(request=="/user-agent"){
          x=request_string.find("User-Agent:");
          x=request_string.find(" ",x+1);
          y=request_string.find("\r",x+1);
          string value=request_string.substr(x+1,y-(x+1));
          http_response="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
          http_response+=to_string(value.length());
          http_response+="\r\n\r\n";
          http_response+=value;
        }
        else if(request.substr(0,7)=="/files/"){
          string filename=request.substr(7);
          string path=directory_path+filename;
          if(method=="POST"){
            ofstream file(path);
            if(!file){
              cout<<"Failed to create the file\n";
            }
            else{
              x=request_string.find("Content-Length:");
              x=request_string.find(" ",x+1);
              y=request_string.find("\r",x+1);
              int length=stoi(request_string.substr(x+1,y-(x+1)));
              y=request_string.find("\r\n\r\n",x+1);
              y+=4;
              string content=request_string.substr(y,length);
              file<<content;
              file.close();
  
              http_response="HTTP/1.1 201 Created\r\n\r\n";
            }
          }
          else if(method=="GET"){
            ifstream file(path, std::ios::binary);
            if(file.is_open()){
              stringstream buff;
              buff << file.rdbuf();
              string file_contents = buff.str();
              http_response="HTTP/1.1 200 OK\r\n";
              http_response+="Content-Type: application/octet-stream\r\n";
              http_response+="Content-Length: "+to_string(file_contents.length())+"\r\n\r\n";
              http_response+=file_contents;
    
            }
            else{
              http_response="HTTP/1.1 404 Not Found\r\n\r\n";
            }
          }
        }
        else{
          http_response = "HTTP/1.1 404 Not Found\r\n\r\n";
         
        }
        write(client_fd, http_response.c_str(), http_response.length());
  
      }
  
      close(client_fd);

    }
  
  }
  else{
    cerr << "Accespt Failed\n";

    return; 
  }  
}

int main(int argc, char **argv) {
  // Extracting the directory path
  string directory_path="."; // It refers to current directory
  if(argc>1 && string(argv[1]) == "--directory"){
    if(argc>2){
      directory_path=argv[2];
    }
  } 
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage
  //
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  //
  // // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  while(true){
    
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    
    std::cout << "Waiting for a client to connect...\n";
    
    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
    // Make a thread to handle concurrent clients
    thread worker_thread(handle_client,client_fd,directory_path);
    worker_thread.detach();
  }

  close(server_fd);
  
  return 0;
}
