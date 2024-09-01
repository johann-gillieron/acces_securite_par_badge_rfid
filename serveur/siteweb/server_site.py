from flask import Flask, render_template, request, redirect, url_for
import mariadb
import random
import string

def generate_user_id():
    return ''.join(random.choices(string.ascii_uppercase + string.digits, k=2))

app = Flask(__name__)

# Configuration de la base de données
config = {
    'user': 'myuser',
    'password': 'password',
    'host': '127.0.0.1',
    'database': 'user_site_web'
}
config2 = {
    'user': 'myuser',
    'password': 'password',
    'host': '127.0.0.1',
    'database': 'user_access_db'
}
@app.route('/')
def login():
    return render_template('login.html')

@app.route('/login', methods=['POST'])
def check_login():
    username = request.form['username']
    password = request.form['password']

    # Connexion à la base de données
    conn = mariadb.connect(**config)
    cursor = conn.cursor()

    # Vérification des identifiants de connexion
    query = "SELECT * FROM users WHERE username = ? AND password = ?"
    cursor.execute(query, (username, password))
    result = cursor.fetchone()

    # Fermeture de la connexion à la base de données
    cursor.close()
    conn.close()

    if result:
        return redirect(url_for('user_management'))
    else:
        return render_template('error.html')

@app.route('/user_management')
def user_management():
    # Connexion à la base de données
    conn = mariadb.connect(**config)
    cursor = conn.cursor()

    # Récupération des utilisateurs
    query = "SELECT * FROM users"
    cursor.execute(query)
    users = cursor.fetchall()

    # Fermeture de la connexion à la base de données
    cursor.close()
    conn.close()

    return render_template('user_management.html', users=users)

@app.route('/user_management2')
def user_management2():
    # Connexion à la base de données
    conn = mariadb.connect(**config2)
    cursor = conn.cursor()

    # Récupération des utilisateurs
    query = "SELECT * FROM users"
    cursor.execute(query)
    users = cursor.fetchall()
    query = "SELECT * FROM users"
    cursor.execute(query)
    users = cursor.fetchall()

    # Fermeture de la connexion à la base de données
    cursor.close()
    conn.close()

    return render_template('user_management2.html', users=users)

@app.route('/add_user_site', methods=['POST'])
def add_user_site():
    username = request.form['username']
    password = request.form['password']

    # Connexion à la base de données
    conn = mariadb.connect(**config)
    cursor = conn.cursor()

    # Ajout d'un nouvel utilisateur
    query = "INSERT INTO users (username, password) VALUES (?, ?)"
    cursor.execute(query, (username, password))

    # Validation de la transaction
    conn.commit()

    # Fermeture de la connexion à la base de données
    cursor.close()
    conn.close()

    return redirect(url_for('user_management'))

@app.route('/add_user', methods=['POST'])
def add_user():
    user_id = generate_user_id()
    first_name = request.form['first_name']
    last_name = request.form['last_name']
    phone = request.form['phone']
    secret = request.form['secret']
    technician = request.form.get('technician') == 'on'
    machine_1 = request.form.get('machine_1') == 'on'
    machine_2 = request.form.get('machine_2') == 'on'

    # Connexion à la base de données
    conn = mariadb.connect(**config2)
    cursor = conn.cursor()

    # Ajout d'un nouvel utilisateur
    query = "INSERT INTO users (user_id, first_name, last_name, phone, secret, technician, machine_1, machine_2) VALUES (?, ?, ?, ?, ?, ?, ? ,?)"
    cursor.execute(query, (user_id, first_name, last_name, phone, secret, technician, machine_1, machine_2))

    # Validation de la transaction
    conn.commit()

    # Fermeture de la connexion à la base de données
    cursor.close()
    conn.close()

    return redirect(url_for('user_management2'))

@app.route('/delete_user/<int:user_id>', methods=['POST'])
def delete_user(user_id):
    # Connexion à la base de données
    conn = mariadb.connect(**config2)
    cursor = conn.cursor()

    # Suppression de l'utilisateur
    query = "DELETE FROM users WHERE id = ?"
    cursor.execute(query, (user_id,))

    # Validation de la transaction
    conn.commit()

    # Fermeture de la connexion à la base de données
    cursor.close()
    conn.close()

    return redirect(url_for('user_management2'))

if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0")