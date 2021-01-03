import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class ClientGUI {
    private JPanel cli;
    private JTable friendList;
    private JTable groupList;
    private JTextField friendName;
    private JButton FRIENDADD;
    private JTextField groupName;
    private JButton GROUPADD;
    private JButton GROUPDELETE;
    private JButton DELETEFRIEND;
    private JButton TALKTOFRIEND;
    private Socket socket;
    private BufferedReader br;
    private PrintWriter pw;

    public ClientGUI() {
    }

    private void sendMsg(String msg) {
        pw.println(msg);
        pw.flush();
    }

    private void flashFriendList() {
        sendMsg("/friendList");
    }

    private void flashGroupList() {
        sendMsg("/groupList");
    }

    private void addFriends() {
        String name = friendName.getText();
        sendMsg("/addFriend " + name);
        friendName.setText("");
    }

    private void addGroup() {
        String name = groupName.getText();
        sendMsg("/addGroup " + name);
        groupName.setText("");
    }

    private void deleteFriend() {
        String name = friendName.getText();
        sendMsg("/deleteFriend " + name);
        friendName.setText("");
    }

    private void deleteGroup() {
        String name = groupName.getText();
        sendMsg("/deleteGroup " + name);
        groupName.setText("");
    }

    private void talkToFriend() {
        String name = friendName.getText();
        sendMsg("/talkToFriend " + name);
        friendName.setText("");
    }

    public ClientGUI(Socket socket) throws IOException {
        this.socket = socket;
        this.pw = new PrintWriter(socket.getOutputStream());
        this.br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        // 处理服务器端的消息
        ClientThread clientThread = null;
        try {
            clientThread = new ClientThread(socket, friendList, groupList);
        } catch (IOException ioException) {
            ioException.printStackTrace();
        }
        clientThread.start();
        flashFriendList();
        flashGroupList();

        FRIENDADD.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                addFriends();
            }
        });
        GROUPADD.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                addGroup();
            }
        });
        DELETEFRIEND.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                deleteFriend();
            }
        });
        GROUPDELETE.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                deleteGroup();
            }
        });
        TALKTOFRIEND.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                talkToFriend();
            }
        });
    }

    public void run(Socket socket) throws IOException {
        JFrame frame = new JFrame("Cl");
        frame.setContentPane(new ClientGUI(socket).cli);
        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosed(WindowEvent e) {
                super.windowClosed(e);
                try {
                    sendMsg("/logout");
                } catch (Exception e1) {
                    e1.printStackTrace();
                }
            }
        });
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

    public static void display() {
        JFrame frame = new JFrame("Cl");
        frame.setContentPane(new ClientGUI().cli);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

    public static void main(String[] args) {
        display();
    }
}
