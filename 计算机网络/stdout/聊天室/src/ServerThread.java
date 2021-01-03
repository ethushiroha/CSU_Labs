import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Iterator;

public class ServerThread extends Thread {
    private User user;
    private ArrayList<User> list;
    private boolean isExited;

    public ServerThread(User user, ArrayList<User> list) {
        this.user = user;
        this.list = list;
        this.isExited = false;
    }

    private boolean inList(String name, ArrayList<User> list) {
        for (User user1 : list) {
            if (user1.getName().equals(name)) {
                return true;
            }
        }
        return false;
    }

    public void sendFriends(User user) throws IOException {
        PrintWriter pw = new PrintWriter(user.getSocket().getOutputStream());
        ArrayList<String> friends = user.getFriends();
        int length = friends.size();
        pw.println("/friendList " + length);
        pw.flush();

        for (String friend : friends) {
            String msg = "";
            msg += friend + ":";
            if (inList(friend, this.list)) {
                msg += "at";
            } else {
                msg += "not";
            }
            pw.println(msg);
            pw.flush();
        }
    }

    private void sendGroup(User user) throws IOException {
        PrintWriter pw = new PrintWriter(user.getSocket().getOutputStream());
        ArrayList<String> group = user.getGroup();
        int length = group.size();
        pw.println("/groupList " + length);
        pw.flush();

        for (String s : group) {
            pw.println(s);
            pw.flush();
        }
    }

    private User FindByName(String name) {
        for (User user1 : list) {
            if (user1.getName().equals(name)) {
                return user1;
            }
        }
        return null;
    }

    @Override
    public void run() {
        user.friends.add(user.getName());
        try {
            while (!isExited) {
                String msg = user.getBr().readLine();
                System.out.println(msg);
                dealMsg(msg);
            }
        } catch (Exception e) {
            System.out.println("异常");
        } finally {
            try {
                user.getBr().close();
                user.getSocket().close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void addFriend(String name) throws IOException {
        for (String friend : user.friends) {
            if (friend.equals(name)) {
                return;
            }
        }
        user.friends.add(name);
        sendFriends(user);
    }

    private void addGroup(String name) throws IOException {
        for (String s : user.group) {
            if (s.equals((name))) {
                return;
            }
        }
        user.group.add(name);
        sendGroup(user);
    }

    private void deleteFriend(String name) throws IOException {
        Iterator<String> iterator = user.friends.iterator();
        while (iterator.hasNext()) {
            String st = iterator.next();
            if (st.equals(name)) {
                iterator.remove();
            }
        }
        sendFriends(user);
    }

    private void deleteGroup(String name) throws IOException {
        Iterator<String> iterator = user.group.iterator();
        while (iterator.hasNext()) {
            String st = iterator.next();
            if (st.equals(name)) {
                iterator.remove();
            }
        }
        sendGroup(user);
    }

    private void remove(User user2) throws IOException {
        user2.getPw().close();
        user2.getBr().close();
        user2.getSocket().close();
        Iterator<User> iterator = list.iterator();
        while (iterator.hasNext()) {
            if (iterator.next() == user2) {
                iterator.remove();
            }
        }
    }

    private void say(String msg, Socket socket) throws IOException {
        PrintWriter pw = new PrintWriter(socket.getOutputStream());
        pw.println(msg);
        pw.flush();
    }

    private void say(User fromuser, User touser, String msg) throws IOException {
        // 是否在线
        System.out.println("now say to both");
        if (inList(touser.getName(), list)) {
            // 是否是好友
            for (String s : fromuser.friends) {
                if (s.equals(touser.getName())) {
                    // 发送消息 给两个人
                    // 格式：/say xx 说了 xx
                    String message = "/say " + fromuser.getName() + " " + msg;

                    say(message, touser.getSocket());
                    say(message, fromuser.getSocket());
                }
            }
        }
    }

    private void searchFriend(User user, String name) {
        PrintWriter pw = user.getPw();
        for (String friend : user.friends) {
            if (friend.equals(name)) {
                pw.println("true");
                pw.flush();
                return;
            }
        }
        pw.println("false");
        pw.flush();
        return;
    }

    private void searchMem(String group) throws IOException {
        int n = 0;
        PrintWriter pw = new PrintWriter(user.getSocket().getOutputStream());
        ArrayList<String> members = new ArrayList<String>();
        for (User user1 : list) {
            ArrayList<String> groupList = user1.getGroup();
            for (String s : groupList) {
                if (s.equals(group)) {
                    n++;
                    members.add(user1.getName());
                }
            }

        }
        pw.println("/getMem " + n);
        pw.flush();

        for (int i = 0; i < members.size(); i++) {
            String msg = members.get(i);
            pw.println(msg);
            pw.flush();
        }
    }

    private void searchGroup(String name) throws IOException {
        PrintWriter pw = new PrintWriter(user.getSocket().getOutputStream());
        for (String s : user.group) {
            if (s.equals(name)) {
                pw.println("true");
                pw.flush();
                return;
            }
        }
        pw.println("false");
        pw.flush();
    }

    private void sayGroup(String fromName, String name, String msg) throws IOException {
        String command = "/sayGroup " + fromName + " " + msg;
        for (User user1 : list) {
            ArrayList<String> group = user1.getGroup();
            for (String s : group) {
                if (s.equals(name)) {
                    say(command, user1.getSocket());
                }
            }
        }
    }

    private void talkToFriend(String name) {
        PrintWriter pw = user.getPw();
        for (int i = 0; i < user.friends.size(); i++) {
            // 是好友
            if (user.friends.get(i).equals(name)) {
                // 在线
                if (inList(name, list)) {
                    pw.println("/talkToFriend " + name);
                    pw.flush();
                    return;
                }
            }
        }
        pw.println("/talkToFriend " + "/failed");
        pw.flush();
    }

    private void dealMsg(String msg) throws IOException {
        String[] str = msg.split(" ");
        switch (str[0]) {
            case "/sayGroup":
                sayGroup(user.getName(), str[1], str[2]);
                break;
            case "/searchGroup":
                searchGroup(str[1]);
                break;
            case "/getMem":
                searchMem(str[1]);
                break;
            case "/searchFriend":
                searchFriend(user, str[1]);
                break;
            case "/talkToFriend":
                talkToFriend(str[1]);
                break;
            case "/say":
                say(user, FindByName(str[1]), str[2]);
                break;
            case "/deleteGroup":
                deleteGroup(str[1]);
                break;
            case "/deleteFriend":
                deleteFriend(str[1]);
                break;
            case "/addGroup":
                addGroup(str[1]);
                break;
            case "/groupList":
                sendGroup(user);
                break;
            case "/addFriend":
                addFriend(str[1]);
                break;
            case "/friendList": // 好友列表
                sendFriends(user);
                break;
            case "/logout":
                remove(user);// 移除用户
                isExited = true;
                break;

            default:
                break;
        }
    }

}
