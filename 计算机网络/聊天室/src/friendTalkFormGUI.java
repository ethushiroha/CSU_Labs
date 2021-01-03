import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;

public class friendTalkFormGUI {
    private JPanel TALK;
    public JTable friendHistory;
    private JTextField INSIDE;
    private JButton SEND;
    public JLabel friendName;
    private JLabel HIS;
    private PrintWriter pw;
    // private friendTalkThread thread;

    public void historyAdd(String[] msg) {
//        String[] col = {"name", "msg"};
//        String[][] row = new String[1][2];
//        row[0] = msg;
        HIS.setText(msg[0]);
//        friendHistory.setEnabled(true);
//        DefaultTableModel defaultTableModel = new DefaultTableModel(row, col);
//        //defaultTableModel.addRow(msg);
//        friendHistory.setModel(defaultTableModel);
//        friendHistory.setEnabled(false);
    }

    private void historyInit() {
        String[] col = {"name", "msg"};
        String[][] row = {};
        DefaultTableModel defaultTableModel = new DefaultTableModel(row, col);
        friendHistory.setModel(defaultTableModel);
        friendHistory.setEnabled(false);
    }

    private void sendToFriend() {
        String inside = INSIDE.getText();
        String name = friendName.getText();
        sendMsg("/say " + name + " " + inside);
    }

    public friendTalkFormGUI(Socket socket, String name) throws IOException {
        this.pw = new PrintWriter(socket.getOutputStream());
        this.friendName.setText(name);
        // this.thread = new friendTalkThread(socket, friendHistory);
        // thread.start();
        historyInit();

        SEND.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                sendToFriend();
            }
        });
    }

    public friendTalkFormGUI() {

    }

    public void sendMsg(String msg) {
        pw.println(msg);
        pw.flush();
    }

    public void display(Socket socket, String name) throws IOException {
        JFrame frame = new JFrame("friendTalkFormGUI");
        frame.setContentPane(new friendTalkFormGUI(socket, name).TALK);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }


}
