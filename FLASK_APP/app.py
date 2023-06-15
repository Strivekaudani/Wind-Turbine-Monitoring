
from flask import Flask, render_template, request, session, logging, url_for, redirect, flash
from passlib.hash import sha256_crypt
from flask_mail import Mail, Message
import sqlite3, time
from datetime import datetime


app = Flask(__name__)

app.config['MAIL_SERVER'] = 'smtp.gmail.com'
app.config['MAIL_PORT'] = 465
app.config['MAIL_USE_SSL'] = True
app.config['MAIL_USERNAME'] = 'windturbinewarning@gmail.com'
app.config['MAIL_PASSWORD'] = 'pdnkocqesmcrhbpi'

mail = Mail(app)

@app.route("/", methods = ['POST', 'GET'])
def index():
    # conn = sqlite3.connect('databasefin.db')
    # c = conn.cursor()  
    
    # c.execute('CREATE TABLE admin (email TEXT, password TEXT)')
    # c.execute('CREATE TABLE turbinedata (id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT, temperature REAL, humidity REAL, vibration REAL, speed REAL)')
    # conn.commit()
    # conn.close()

    if request.method == 'POST':

        contact_name = request.form.get("contactname")
        contact_email = request.form.get("contactemail")
        contact_subject = request.form.get("contactsubject")
        contact_msg = request.form.get("contactmsg")

        e_mail = 'tapiwanashemakani99@gmail.com'
        # e_mail = 'mskaudani@gmail.com'
        msg = Message(contact_subject, sender = 'windturbinewarning@gmail.com', recipients = [e_mail])
        msg.body = "You have a message from " + contact_email + "\n\n" + contact_msg + "\n\n" + contact_name
        mail.send(msg)

    return render_template("indexx.html")

@app.route("/signin", methods = ['POST', 'GET'])
def signin():
    return render_template("signin.html")

@app.route("/post", methods = ['POST'])
def post():
    print(request.data)
    dayta = request.data
    data_ = dayta.decode('utf-8')
    data_array = data_.split(',')
    
    speed = data_array[0]
    vibration = data_array[1]
    humidity = data_array[2]
    temperature = data_array[3]

    print(speed + "," + vibration + "," + humidity + "," + temperature)

    conn = sqlite3.connect('databasefin.db')
    c = conn.cursor()  

    sql_command = "INSERT INTO turbinedata (date, temperature, humidity, vibration, speed) VALUES (?, ?, ?, ?, ?)"
    values = (datetime.now(), temperature, humidity, vibration, speed,)
    c.execute(sql_command, values)

    conn.commit()
    conn.close()

    max_temp = 30.0
    max_humidity = 85.0
    max_vibration = 3000.0
    max_speed = 180.0

    e_mail = 'tapiwanashemakani99@gmail.com'
    msg = Message('Danger Warning', sender = 'windturbinewarning@gmail.com', recipients = [e_mail])      

    if float(temperature) > max_temp:
        msg.body = "Your wind turbine is operating at a dangerous temperature of " + temperature + ". This may be caused by overheating of turbine and premature wear caused by unpredictable stress or a faulty sensor."
        mail.send(msg)
    elif float(humidity) > max_humidity:
        msg.body = "Your wind turbine is operating at a dangerous humidity of " + humidity + ". This may be caused by corrosion of traces, oil leakages or a faulty sensor."
        mail.send(msg)
    elif float(vibration) > max_vibration:
        msg.body = "Your wind turbine is operating at a dangerous vibration of " + vibration + ". This may be caused by excessive vibrations of generator, misaligned blades and rotors or a faulty sensor."
        mail.send(msg)
    elif float(speed) > max_speed:
        msg.body = "Your wind turbine is operating at a dangerous speed of " + speed + ". This may be caused by hydraulic failures, coupling failures or a faulty sensor."
        mail.send(msg)

    return ''

@app.route("/welcome", methods = ["GET", "POST"])
def dashboard():
    if request.method == 'POST':

        conn = sqlite3.connect('databasefin.db')
        c = conn.cursor()

        email = request.form.get("email")
        password = request.form.get("password")

        email_data = c.execute("SELECT email FROM admin WHERE email = ?", [email]).fetchone()
        password_data = c.execute("SELECT password FROM admin WHERE email = ?", [email]).fetchone()
        print(password_data)

        for pass_data in password_data:
            print(pass_data) 
            
            if sha256_crypt.verify(password, pass_data):
                wild_dataset = c.execute("SELECT * FROM turbinedata ORDER BY id DESC LIMIT 8").fetchall()
                print(wild_dataset)

                dataset_one = wild_dataset[0]
                dataset_two = wild_dataset[1]
                dataset_three = wild_dataset[2]
                dataset_four = wild_dataset[3]
                dataset_five = wild_dataset[4]
                dataset_six = wild_dataset[5]
                dataset_seven = wild_dataset[6]
                dataset_eight = wild_dataset[7]

                date_one = dataset_one[1]
                temperature_one = float(dataset_one[2])
                humidity_one = float(dataset_one[3])
                vibration_one = float(dataset_one[4])
                speed_one = float(dataset_one[5])

                date_two = dataset_two[1]
                temperature_two = float(dataset_two[2])
                humidity_two = float(dataset_two[3])
                vibration_two = float(dataset_two[4])
                speed_two = float(dataset_two[5])

                date_three = dataset_three[1]
                temperature_three = float(dataset_three[2])
                humidity_three = float(dataset_three[3])
                vibration_three = float(dataset_three[4])
                speed_three = float(dataset_three[5])

                date_four = dataset_four[1]
                temperature_four = float(dataset_four[2])
                humidity_four = float(dataset_four[3])
                vibration_four = float(dataset_four[4])
                speed_four = float(dataset_four[5])

                date_five = dataset_five[1]
                temperature_five = float(dataset_five[2])
                humidity_five = float(dataset_five[3])
                vibration_five = float(dataset_five[4])
                speed_five = float(dataset_five[5])

                date_six = dataset_six[1]
                temperature_six = float(dataset_six[2])
                humidity_six = float(dataset_six[3])
                vibration_six = float(dataset_six[4])
                speed_six = float(dataset_six[5])

                date_seven = dataset_seven[1]
                temperature_seven = float(dataset_seven[2])
                humidity_seven = float(dataset_seven[3])
                vibration_seven = float(dataset_seven[4])
                speed_seven = float(dataset_seven[5])

                date_eight = dataset_eight[1]
                temperature_eight = float(dataset_eight[2])
                humidity_eight = float(dataset_eight[3])
                vibration_eight = float(dataset_eight[4])
                speed_eight = float(dataset_eight[5])

                max_temp = 30.0
                max_humidity = 85.0
                max_vibration = 4000.0
                max_speed = 180.0
     
                return render_template("dashboard.html",
                      date_one=date_one, temperature_one=temperature_one, humidity_one=humidity_one, vibration_one=vibration_one, speed_one=speed_one, 
                      date_two=date_two, temperature_two=temperature_two, humidity_two=humidity_two, vibration_two=vibration_two, speed_two=speed_two,
                      date_three=date_three, temperature_three=temperature_three, humidity_three=humidity_three, vibration_three=vibration_three, speed_three=speed_three,
                      date_four=date_four, temperature_four=temperature_four, humidity_four=humidity_four, vibration_four=vibration_four, speed_four=speed_four,
                      date_five=date_five, temperature_five=temperature_five, humidity_five=humidity_five, vibration_five=vibration_five, speed_five=speed_five,
                      date_six=date_six, temperature_six=temperature_six, humidity_six=humidity_six, vibration_six=vibration_six, speed_six=speed_six,
                      date_seven=date_seven, temperature_seven=temperature_seven, humidity_seven=humidity_seven, vibration_seven=vibration_seven, speed_seven=speed_seven,
                      date_eight=date_eight, temperature_eight=temperature_eight, humidity_eight=humidity_eight, vibration_eight=vibration_eight, speed_eight=speed_eight,
                      max_humidity=max_humidity, max_speed=max_speed, max_vibration=max_vibration, max_temp=max_temp           
                                       )
            else:
                return render_template("signin.html")

    return render_template("dashboard.html")

if __name__ == "__main__":
    app.run(host='0.0.0.0', port= 8090, debug=True)
    # app.run(debug=True)
    app.secret_key = "tapiwa"


    # c.execute('CREATE TABLE admin (email TEXT, password TEXT)')

    # emailll = 'tapiwanashemakani99@gmail.com'
    # passworddd = 'tapiwa'
    # s_password = sha256_crypt.hash(str(passworddd))
    # print(s_password)
    # sql_command = "INSERT INTO admin (email, password) VALUES (?, ?)"
    # values = (emailll, s_password)
    # c.execute(sql_command, values)
    # sql_command = "INSERT INTO sensordata (temperature, humidity, vibration, speed) VALUES (?, ?, ?, ?)"
    # values = (one, two, three, four,)
