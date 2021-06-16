/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package homework4.server;

import java.io.*;
import java.util.*;
import java.net.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author nahom
 */

public class Homework4Server {
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        final int port = 5190;
        new Server(port);
    }
}

class Server {
    ServerSocket servSock;
    ArrayList<Socket> clients;
    Server(final int port){
        clients = new ArrayList<Socket>();
        try {
            servSock = new ServerSocket(port);
        } catch (IOException ex) {
            System.out.println("failed to create server");
            System.exit(1);
        }
        System.out.println("listening on port: " + port + "\n");
        
        listen();
    }
    
    private void listen(){
        while (true){
            try {
                Socket clientSock = servSock.accept();
                clients.add(clientSock);
                
                //thread
                new ProcessConnection(clientSock).start();
                
            } catch (IOException ex) {
                System.out.println("failed to accept a client");
            }
            
            
        }
    }
    
    synchronized private void displayToClients(String msg, String selfName) throws IOException {
        for (Socket client : clients){
            PrintStream ps = new PrintStream(client.getOutputStream());
            ps.println(selfName + ": " + msg);
        }
    }
    
    class ProcessConnection extends Thread {
        Socket clientSock;
        String clientName;
        
        ProcessConnection(Socket _clientSock){
            clientName = "";
            clientSock = _clientSock;
        }
        
        public void run(){
            try{
                Scanner sin = new Scanner(clientSock.getInputStream());
                PrintStream ps = new PrintStream(clientSock.getOutputStream());
                
                if (clientName.equalsIgnoreCase("")){ //simple check: good programming trait
                    clientName = sin.nextLine();
                }
                
                String line = "";
                while (!line.equalsIgnoreCase("EXIT")){
                    if (sin.hasNext()){
                        line = sin.nextLine();
                    }
                    if (line.equalsIgnoreCase("\n") || line.equalsIgnoreCase("") || line.equalsIgnoreCase("EXIT")){
                        continue; //skip empty inputs
                    }
                    displayToClients(line, clientName);    
                }
                clientSock.close();
                clients.remove(clientSock);
            }
            catch (IOException ex){}
        }
    }
}