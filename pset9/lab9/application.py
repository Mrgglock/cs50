import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///birthdays.db")

@app.route("/", methods=["GET", "POST"])
def index():
    # Get int representations of months to strs dict here.
    monthName = getMonths()
    if request.method == "POST":

        # TODO: Add the user's entry into the database
        name = request.form.get("name")
        birthday = request.form.get("birthday")
        year, month, day = birthday.split("-")
        month = int(month)
        day = int(day)
        db.execute("INSERT INTO birthdays (name, month, day) VALUES (?, ?, ?)", name, month, day)
        return redirect("/")

    else:

        # TODO: Display the entries in the database on index.html
        data = db.execute("SELECT name, month, day FROM birthdays;")

        for person in data:
            person["birthday"] = str(person["day"]) + " " + monthName[person["month"]]

        return render_template("index.html", data=data)

def getMonths():
    return {
        1: "Jan",
        2: "Feb",
        3: "Mar",
        4: "Apr",
        5: "May",
        6: "Jun",
        7: "Jul",
        8: "Aug",
        9: "Sep",
        10: "Oct",
        11: "Nov",
        12: "Dec"
    }