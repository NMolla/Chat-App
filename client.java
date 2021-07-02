/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package homework4.client;

import java.io.*;
import java.util.*;
import java.net.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 *
 * @author nahom
 */
public class client {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        new Client();
    }
}

// Client
// ==========================================================================

class Client {
    private Socket client;
    private String name, ip; //new
    private GUI gui;
    private PrintStream out;
    private Scanner in;
    private final int port=5190;
    
    //Constructor
    Client(){    
        initialize();
    }
    
    //private methods
    private void initialize(){
        JFrame frame = new JFrame("Enter Name & IP");
        frame.setSize(300, 100);
        frame.setMinimumSize(new Dimension(300, 100));
        frame.setMaximumSize(new Dimension(300, 100));
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        JPanel panel = new JPanel();
        panel.setLayout(new GridLayout(2, 1));
        
        JPanel topPanel = new JPanel();
        JLabel jl = new JLabel("Name:");
        JTextField jtf = new JTextField();
        jtf.setText("J. Chatter");
        topPanel.setLayout(new BorderLayout());
        topPanel.add(jl, BorderLayout.WEST);
        topPanel.add(jtf, BorderLayout.CENTER);
        
        JPanel bottomPanel = new JPanel();
        JLabel jl2 = new JLabel("ip:");
        JTextField jtf2 = new JTextField();
        jtf2.setText("127.0.0.1");
        bottomPanel.setLayout(new BorderLayout());
        bottomPanel.add(jl2, BorderLayout.WEST);
        bottomPanel.add(jtf2, BorderLayout.CENTER);
        
        JButton send = new JButton("Launch");
        send.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                name = jtf.getText();
                ip = jtf2.getText();
                frame.dispose();
                setup(); //open new GUI after obtaining name & ip
            }
        });
        
        bottomPanel.add(send, BorderLayout.EAST);
        
        panel.add(topPanel, BorderLayout.NORTH);
        panel.add(bottomPanel, BorderLayout.SOUTH);
        
        frame.add(panel, BorderLayout.CENTER);
        frame.setVisible(true);
    }
    
    private void setup(){
        gui = new GUI(this);
        
        try {            
            client = new Socket(ip, port);
            System.out.println("\n==== connected to " + ip + ":" + port +" ====\n");
            
            out = new PrintStream(client.getOutputStream());
            in = new Scanner(client.getInputStream());
            
        } catch (IOException ex) {
            System.out.println("\n==== failed to connect to server ====\n");
        }
        
        //send name
        out.println(name);
        
        //listen on the in-stream
        new SocketListener(in).start();
    }
    
    void callSendListener(JTextField jtf){ //package private
        new SendListener(jtf).start();
    }
    
    //sub-classes
    private class SocketListener extends Thread {
        Scanner in;

        SocketListener(Scanner inStream){
            in = inStream;
        }

        public void run(){
            String line = "";
            while (in.hasNextLine()){
                line = in.nextLine();
                gui.displayMsg(line);
            }
        }
    } 
    
    private class SendListener extends Thread {
        JTextField jtf;
        
        SendListener(JTextField _jtf){
            jtf = _jtf;
        }
        
        public void run(){
            String line = jtf.getText();
            jtf.setText("");
            
            out.println(line);
            
            if (line.equalsIgnoreCase("EXIT")){
                try {
                    client.close();
                    gui.close();
                    System.out.println("\n==== connection terminated ====\n");
                    
                } catch (IOException ex) {
                    System.out.println("\n==== failed to close client on EXIT ====\n");
                }
            }
        }
    }
}

//GUI
// ==========================================================================

class GUI{
    private Client client;
    private JFrame frame;
    private JScrollPane scrollPane;
    private JTextArea textArea;
    private JPanel lowerPane;
    private JTextField textField;
    private JButton sendButton;
    
    //Constructor
    GUI(Client _client){
        client = _client;
        initialize();
        frame.pack();
        frame.setVisible(true);
    }
    
    //methods
    private void initialize(){
        frame = new JFrame("The Chat App!");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(500, 500);
        frame.setMinimumSize(new Dimension(200, 200));
                
        textArea = new JTextArea();
        textArea.setEditable(false);
        textArea.setLineWrap(true);
        textArea.setWrapStyleWord(true);
        
        scrollPane = new JScrollPane(textArea);
        scrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        scrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        scrollPane.setPreferredSize(new Dimension(400, 400));
        
        lowerPane = new JPanel();
        lowerPane.setLayout(new BorderLayout());
        
        textField = new JTextField();
        sendButton = new JButton("SEND");
        
        textField.setEditable(true); 
        textField.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                sendButton.doClick();
            }
        });
        
        sendButton.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                client.callSendListener(textField);
            }
        });
        
        lowerPane.add(textField, BorderLayout.CENTER);
        lowerPane.add(sendButton, BorderLayout.EAST);
        
        frame.add(scrollPane, BorderLayout.CENTER);
        frame.add(lowerPane, BorderLayout.SOUTH);
    }
    
    void displayMsg(String msg){ //package private
        //display server message in text area
        textArea.append(msg+"\n");
    }
    
    void close(){ //package private
        frame.dispose();
    }
}
