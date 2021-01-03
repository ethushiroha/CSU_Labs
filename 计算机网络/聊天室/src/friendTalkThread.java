import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class friendTalkThread extends Thread{
    private Socket socket;
    private PrintWriter pw;
    private BufferedReader br;
    private JTable history;

    public friendTalkThread(Socket socket, JTable history) throws IOException {
        this.socket = socket;
        this.history = history;
        this.pw = new PrintWriter(socket.getOutputStream());
        this.br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    }

    private void addHistory(String[] msg) {
        history.setEnabled(true);
        DefaultTableModel defaultTableModel = (DefaultTableModel) history.getModel();
        String[] row = { msg[1], msg[2] };
        defaultTableModel.addRow(row);
        history.setModel(defaultTableModel);
        history.setEnabled(false);
    }

    private void delmsg(String msg) throws IOException {
        String[] str = msg.split(" ");
        switch (str[0]) {
            case "/say":
                addHistory(str);
                break;
            default:
                break;
        }
    }

    @Override
    public void run() {
        try {
            while (true) {
                String msg = br.readLine();
                delmsg(msg);
            }
        } catch (IOException ioException) {
            ioException.printStackTrace();
        }
    }
}
