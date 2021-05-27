import os
import time
from datetime import datetime, timezone

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    # Initialise a list, to be displayed as rows.
    stocks = []

    def createRow(symbol, name, shares, price, total):
        if not price == "":
            price = usd(price)
        if not total == "":
            total = usd(total)
        stocks.append({"symbol": symbol,
                "name": name,
                "shares": shares,
                "price": price,
                "total": total
        })
        return

    # Get user's current cash and add it
    cash = db.execute("SELECT * FROM users WHERE id = ?", session["user_id"])[0]["cash"]
    totalCash = cash
    createRow("CASH", "", "", "", cash)

    # Get all the user's current stocks, sort by alphabetical order
    ownedStocks = db.execute("SELECT * FROM shares WHERE userid = ?", session["user_id"])
    ownedStocks.sort(key=lambda x:x["symbol"])

    # Loop through each stock and generate rows
    for ownedStock in ownedStocks:
        symbol = ownedStock["symbol"]
        shares = ownedStock["shares"]
        # Find the name of this stock and its price
        searched = lookup(symbol)
        name = searched["name"]
        price = searched["price"]
        # Calculate total value and subtract from wallet
        totalValue = price * shares
        cash -= totalValue
        totalCash += totalValue
        createRow(symbol, name, shares, price, totalValue)

    return render_template("index.html", stocks=stocks, finalTotal=usd(totalCash))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    if request.method == "POST":
        # User reached route via POST

        # Get symbol and number of shares
        symbol = request.form.get("symbol").strip()
        shares = 0
        try:
            shares = int(request.form.get("shares"))
        except:
            return apology("number is not an int")
        response = lookup(symbol)

        # Validate response
        if response == None:
            return apology("symbol doesn't exist")
        if shares <= 0:
            return apology("input is not a positive integer")

        # Buy stocks

        # Calculate price
        cost = response["price"] * shares

        # Get user's wallet
        user = db.execute("SELECT * FROM users WHERE id = ?;", session["user_id"])
        wallet = user[0]["cash"]

        # Make sure they has enough money
        if wallet < cost:
            return apology("cannot afford number of shares")

        # Complete purchase

        # Take money
        db.execute("UPDATE users SET cash = ? WHERE id = ?;", wallet - cost, session["user_id"])

        # Give stocks
        # See if they already own this stock, if they do, just update this instead
        hasStock = db.execute("SELECT * FROM shares WHERE userid = ? AND symbol = ?;", session["user_id"], response["symbol"])
        if len(hasStock) == 0:
            # They do not own this stock. Add to database
            db.execute("INSERT INTO shares (userid, symbol, shares) VALUES (?, ?, ?);", session["user_id"], response["symbol"], shares)
        else:
            # They already own this stock. Update instead.
            existingStocks = hasStock[0]["shares"]
            db.execute("UPDATE shares SET shares = ? WHERE userid = ? AND symbol = ?;", existingStocks + shares, session["user_id"], response["symbol"])

        # Update history
        now = int(time.time())
        db.execute("INSERT INTO history (userid, transaction_type, symbol, price, shares, time) VALUES (?, ?, ?, ?, ?, ?)", session["user_id"], "buy", response["symbol"], response["price"], shares, now)

        # Everything is done; redirect
        return redirect("/")
    else:
        # User reached route via GET
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    # Get user's stock history
    history = db.execute("SELECT * FROM history WHERE userid = ?", session["user_id"])

    # Change time to timestamp
    for row in history:
        row["time"] = datetime.fromtimestamp(row["time"]).strftime('%Y-%m-%d %H:%M:%S')
        row["transaction_type"] = row["transaction_type"].capitalize()

    return render_template("history.html", history=history)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    if request.method == "POST":
        # User reached route via POST
        symbol = request.form.get("symbol")
        response = lookup(symbol)
        if response == None:
            return apology("invalid symbol")
        return render_template("quoted.html", search=symbol, response=response)
    else:
        # User reached route via GET
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    if request.method == "POST":
        # User reached route via POST

        # Get credentials
        username = request.form.get("username").strip()
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        # Validate input
        if (username == ""):
            return apology("missing username")
        if (password == ""):
            return apology("missing password")
        if (password != confirmation):
            return apology("passwords do not match")

        # Make sure someone else doesn't have this username already.
        users = db.execute("SELECT * FROM users WHERE username = ?;", username)
        if (len(users) > 0):
            return apology("username already exists")

        # All checks are done and input is good.

        hash = generate_password_hash(password)
        # Insert the new user.
        db.execute("INSERT INTO users (username, hash) VALUES (?, ?)", username, hash)

        # Redirect them, logging in for them.

        # Clear session, look for their ID, store in session, redirect to home page.
        session.clear()
        rows = db.execute("SELECT * FROM users WHERE username = ?", username)
        session["user_id"] = rows[0]["id"]
        return redirect("/")

    else:
        # User reached route via GET
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    if request.method == "POST":
        # User reached route via POST

        # Get form data
        symbol = request.form.get("symbol")

        soldShares = 0
        try:
            soldShares = int(request.form.get("shares"))
        except:
            return apology("number is not an int")

        if soldShares <= 0:
            return apology("number of shares is not a positive integer")

        # Look for symbol in stock market
        response = lookup(symbol)
        if response == None:
            return apology("symbol doesn't exist")

        # Check if user owns the stock and has enough
        ownedStocks = db.execute("SELECT * FROM shares WHERE userid = ? AND symbol = ?", session["user_id"], symbol)
        if len(ownedStocks) == 0:
            return apology("you do not own this stock")
        ownedShares = ownedStocks[0]["shares"]

        if ownedShares < soldShares:
            return apology("you cannot sell more stocks than you own")

        # Stock exists, requested amount is valid, and owns enough. Complete selling
        value = response["price"] * soldShares

        # Give money
        wallet = db.execute("SELECT * FROM users WHERE id = ?", session["user_id"])[0]["cash"]
        db.execute("UPDATE users SET cash = ? WHERE id = ?", wallet + value, session["user_id"])

        # Remove from collection

        if ownedShares == soldShares:
            # If the user sold exactly as many shares as he owns, remove the entry completely from the db.
            db.execute("DELETE FROM shares WHERE symbol = ? AND userid = ?", response["symbol"], session["user_id"])
        else:
            # Subtract as many as he needs
            db.execute("UPDATE shares SET shares = ? WHERE symbol = ? AND userid = ?", ownedShares - soldShares, response["symbol"], session["user_id"])

        # Update history
        now = int(time.time())
        db.execute("INSERT INTO history (userid, transaction_type, symbol, price, shares, time) VALUES (?, ?, ?, ?, ?, ?)", session["user_id"], "sell", response["symbol"], response["price"], soldShares, now)

        return redirect("/")
    else:
        # User reached route via GET

        # Display the user's owned stocks for selection to sell.
        stocks = db.execute("SELECT * FROM shares WHERE userid = ?", session["user_id"])
        return render_template("sell.html", stocks=stocks)

@app.route("/deposit", methods=["GET", "POST"])
@login_required
def deposit():
    """Add money to account"""

    if request.method == "POST":
        # User reached route via POST
        money = request.form.get("deposit")

        try:
            money = float(money)
        except:
            return apology("money is of incorrect format")

        if money <= 0:
            return apology("money should be positive")

        wallet = db.execute("SELECT * FROM users WHERE id = ?", session["user_id"])[0]["cash"]
        db.execute("UPDATE users SET cash = ? WHERE id = ?", wallet + money, session["user_id"])

        return redirect("/")
    else:
        # User reached route via GET
        return render_template("deposit.html")

def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
