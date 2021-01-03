import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class LoginGUI {
    private JButton login;
    private JTextField usr;
    private JTextField pwd;
    private JLabel status;
    private JPanel LoginGUI;

    public LoginGUI(JFrame frame) {
        login.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String usrname = usr.getText();
                String passwd = pwd.getText();

                Client client = new Client();
                try {
                    Socket socket = client.connects("127.0.0.1",8848);
                    client.sendMsg(socket, usrname + ":" + passwd);
                    BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    String st = br.readLine();

                    if (st.equals("success")) {
                        frame.dispose();
                        // 登录成功
                        ClientGUI cl = new ClientGUI();
                        cl.run(socket);
                    } else {
                        pwd.setText("");
                        status.setText("用户名或密码错误");
                        socket.close();
                    }
                }  catch (IOException interruptedException) {
                    interruptedException.printStackTrace();
                }
            }
        });
    }

    public static void main(String[] args) {
        JFrame frame = new JFrame("Login");
        frame.setContentPane(new LoginGUI(frame).LoginGUI);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);

    }
}
