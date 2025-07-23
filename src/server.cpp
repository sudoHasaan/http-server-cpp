#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

int main(int argc, char **argv) {
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
  
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  
  std::cout << "Waiting for a client to connect...\n";
  
  int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  if (client_fd > 0){
    std::cout << "Client connected\n";
    char buffer[1024];
    int bytes_recieved=read(client_fd, buffer, sizeof(buffer)-1);
    if(bytes_recieved<0){
      cerr<<"Failed to read from server\n";
      return 0;
    }
    else{
      string request_string(buffer);
      size_t x=request_string.find(" "); // start pos of response
      size_t y=request_string.find(" ",x+1); // end pos of response
      string http_response; // the response i will send
      string request=request_string.substr(x+1,y-(x+1)); // getting the request to give the appropriate response.
      
      if(request=="/"){
        http_response = "HTTP/1.1 200 OK\r\n\r\n";
      }
      else if(request.substr(0, 6)=="/echo/"){
      
        // if request contains the word "echo", then print the word after in in your response
        
        x=request.find("/",1);
        y=y=request_string.find(" ",x+1);
        
        string echo_str=request.substr(x+1,y-(x+1));
        http_response="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
        http_response+=to_string(echo_str.length());
        http_response+="\r\n\r\n";
        http_response+=echo_str;
        
      }
      else if(request=="/user-agent"){
        x=request_string.find("User-Agent:");
        x=request_string.find(" ",x+1);
        y=request_string.find("\r",x+1);
        string value=request_string.substr(x+1,y-x);
        http_response="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
        http_response+=to_string(value.length()-1);
        http_response+="\r\n\r\n";
        http_response+=value;
      }
      else{
        http_response = "HTTP/1.1 404 Not Found\r\n\r\n";
       
      }
      write(client_fd, http_response.c_str(), http_response.length());

    }

    close(client_fd);
  
  }
  else{
    cerr << "Accespt Failed\n";

    return 0; 
  }
  
  close(server_fd);

  return 0;
}
