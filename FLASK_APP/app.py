from flask import Flask, render_template, request, session, logging, url_for, redirect, flash
from passlib.hash import sha256_crypt
from flask_mail import Mail, Message
import sqlite3

app = Flask(__name__)

app.config['MAIL_SERVER'] = 'smtp.gmail.com'
app.config['MAIL_PORT'] = 465
app.config['MAIL_USE_SSL'] = True
app.config['MAIL_USERNAME'] = 'windturbinewarning@gmail.com'
app.config['MAIL_PASSWORD'] = 'pdnkocqesmcrhbpi'

mail = Mail(app)

@app.route("/", methods = ['POST', 'GET'])
def index():
    conn = sqlite3.connect('database.db')
    print("Opened database successfully");

    c = conn.cursor()

    c.execute('CREATE TABLE admin (email TEXT, password TEXT)')
    print("Table created successfully");
    conn.commit()
    conn.close()

    if request.method == 'POST':
        # dayta = request.data
        # conn = psycopg2.connect(
        #     host = "ec2-34-227-120-79.compute-1.amazonaws.com",
        #     database = "d5gdmmvca8huis",
        #     user = "wspdbziqddzcys",
        #     password = "e91763dbf0329d5744c1474ec5ca84a39d80fe4e7c4e86cf71073ffbbab1dc53",
        #     port = "5432",
        # )

        # c = conn.cursor()

        # sql_command = "INSERT INTO datatable (data) VALUES (%s)"
        # values = (dayta,)
        # c.execute(sql_command, values)

        # conn.commit()
        # conn.close()
        e_mail = 'mskaudani@gmail.com'
        msg = Message('Wind Turbine Contact Us', sender = 'windturbinewarning@gmail.com', recipients = [e_mail])
        msg.body = "Hie " + nom + " " + surnom + "! The password for your AutoFlood account is " + paass + " \n\nRegards, \n\nThe AutoFlood Team"
        mail.send(msg)

    return render_template("indexx.html")

@app.route("/signin", methods = ['POST', 'GET'])
def signin():
    return render_template("signin.html")

@app.route("/post", methods = ['POST'])
def post():
    dayta = request.data
    conn = psycopg2.connect(
        host = "ec2-34-227-120-79.compute-1.amazonaws.com",
        database = "d5gdmmvca8huis",
        user = "wspdbziqddzcys",
        password = "e91763dbf0329d5744c1474ec5ca84a39d80fe4e7c4e86cf71073ffbbab1dc53",
        port = "5432",
    )

    c = conn.cursor()

    sql_command = "INSERT INTO datatable (data) VALUES (%s)"
    values = (dayta,)
    c.execute(sql_command, values)

    conn.commit()
    conn.close()

    return ''

@app.route("/welcome", methods = ["GET", "POST"])
def dashboard():
    if request.method == 'POST':

        conn = psycopg2.connect(
            host = "ec2-34-227-120-79.compute-1.amazonaws.com",
            database = "d5gdmmvca8huis",
            user = "wspdbziqddzcys",
            password = "e91763dbf0329d5744c1474ec5ca84a39d80fe4e7c4e86cf71073ffbbab1dc53",
            port = "5432",
        )

        c = conn.cursor()

        email = request.form.get("email")
        password = request.form.get("password")

        # c.execute("SELECT * FROM users")
        # records = c.fetchall()
        #
        # word = ''
        # for record in records:


        # email_data = c.execute("SELECT email FROM user WHERE email = :email", {"email":email}).fetchone()
        # password_data = c.execute("SELECT password FROM user WHERE email = :email", {"email":email}).fetchone()

        email_data = c.execute("SELECT email FROM users WHERE email = %s", [email]).fetchone()
        password_data = c.execute("SELECT password FROM users WHERE email = %s", [email]).fetchone()

        for pass_data in password_data:
            if sha256_crypt.verify(password, pass_data):
                return render_template("dashboard.html")
            else:
                return render_template("signin.html")

    return render_template("dashboard.html")

if __name__ == "__main__":
    # app.run(host='0.0.0.0', port= 8090, debug=True)
    app.run()
    app.secret_key = "tapiwa"
