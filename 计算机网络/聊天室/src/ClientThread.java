import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.io.*;
import java.net.Socket;

public class ClientThread extends Thread {
    private Socket socket;
    private BufferedReader br;
    private PrintWriter pw;
    private JTable friendList;
    private JTable groupList;
    private JTable history;
    private friendTalkFormGUI f;

    private void friendListInit() {
        String[] col = {"name", "status"};
        String[][] row = {};
        DefaultTableModel defaultTableModel = new DefaultTableModel(row, col);
        friendList.setModel(defaultTableModel);
        friendList.setEnabled(false);
    }

    private void groupListInit() {
        String[] col = {"name"};
        String[][] row = {};
        DefaultTableModel defaultTableModel = new DefaultTableModel(row, col);
        groupList.setModel(defaultTableModel);
        groupList.setEnabled(false);
    }

    public  ClientThread(Socket socket, JTable friendList, JTable groupList) throws IOException {
        this.socket = socket;
        this.friendList = friendList;
        this.groupList = groupList;
        this.br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        this.pw = new PrintWriter(socket.getOutputStream());
        friendListInit();
        groupListInit();
    }

    private void flashFriendList(int n) throws IOException {
        String[][] row = new String[n][2];
        friendList.setEnabled(true);
        String[] col = {"name", "status"};
        for (int i = 0; i < n; i++) {
            String str = br.readLine();
            row[i] = str.split(":");
        }
        DefaultTableModel defaultTableModel = new DefaultTableModel(row, col);
        friendList.setModel(defaultTableModel);
        friendList.setEnabled(false);
    }

    private void flashGroupList(int n) throws IOException {
        groupList.setEnabled(true);
        String[][] row = new String[n][1];
        String[] col = {"name"};
        for (int i = 0; i < n; i++) {
            String str = br.readLine();
            row[i][0] = str;
        }
        DefaultTableModel defaultTableModel = new DefaultTableModel(row, col);
        groupList.setModel(defaultTableModel);
        groupList.setEnabled(false);
    }

    private void talkToFriend(String status) throws IOException {
        if (status.equals("/failed")) {
            return;
        } else {
            this.f = new friendTalkFormGUI();
            f.display(socket, status);
            this.history = f.friendHistory;
        }
    }

    private void delSay(String name, String msg) {
        String[] st = {name, msg};
        history.setEnabled(true);
        DefaultTableModel defaultTableModel = (DefaultTableModel) history.getModel();
        defaultTableModel.addRow(st);
        this.history.setModel(defaultTableModel);
        history.setEnabled(false);
    }

    private void dealMsg(String msg) throws IOException {
        String[] str = msg.split(" ");
        switch (str[0]) {
            case "/say":
                delSay(str[1], str[2]);
                break;
            case "/talkToFriend":
                talkToFriend(str[1]);
                break;
            case "/groupList":
                flashGroupList(Integer.parseInt(str[1]));
                break;
            case "/friendList":
                flashFriendList(Integer.parseInt(str[1]));
                break;

            default:
                break;
        }
    }

    @Override
    public void run() {
        try {
            try {
                while (true) {
                    String msg = br.readLine();
                    // debug
                    System.out.println(msg);
                    dealMsg(msg);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
