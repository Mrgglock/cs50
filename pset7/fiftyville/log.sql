-- Keep a log of any SQL queries you execute as you solve the mystery.

-- Select all crime scene reports that happened on July 28, 2020 at Chamberlin Street.
SELECT description FROM crime_scene_reports
    WHERE year = 2020
    AND month = 7
    AND day = 28
    AND street = "Chamberlin Street";

/*
Returns:
Theft of the CS50 duck took place at 10:15am at the Chamberlin Street courthouse. Interviews were conducted today with three witnesses
who were present at the time — each of their interview transcripts mentions the courthouse.
*/

-- Select all interviews that happened on the day of the theft which also mentions the courthouse.
SELECT name, transcript FROM interviews
    WHERE transcript LIKE "%courthouse%"
    AND year = 2020
    AND month = 7
    AND day = 28;

/*
Returns:
Ruth | Sometime within ten minutes of the theft, I saw the thief get into a car in the courthouse parking lot and drive away. If you have security footage from the courthouse parking lot, you might want to look for cars that left the parking lot in that time frame.
Eugene | I don't know the thief's name, but it was someone I recognized. Earlier this morning, before I arrived at the courthouse, I was walking by the ATM on Fifer Street and saw the thief there withdrawing some money.
Raymond | As the thief was leaving the courthouse, they called someone who talked to them for less than a minute. In the call, I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow. The thief then asked the person on the other end of the phone to purchase the flight ticket.
*/

-- Based on Ruth's claim, select the license plates exiting from the courthouse on the day of the theft within 10min of 10:15.
SELECT license_plate FROM courthouse_security_logs
    WHERE activity = "exit"
    AND year = 2020
    AND month = 7
    AND day = 28
    AND hour = 10
    AND minute >= 15
    AND minute <= 25;

/*
Returns:
A bunch of license plates.
*/

-- Based on these license plates, find people's names attached to them.
SELECT id FROM people
    JOIN courthouse_security_logs ON people.license_plate = courthouse_security_logs.license_plate
    WHERE activity = "exit"
    AND year = 2020
    AND month = 7
    AND day = 28
    AND hour = 10
    AND minute >= 15
    AND minute <= 25;

/*
Returns:
A list of a suspicious thief IDs.
*/

-- Based on Eugene's claim, select useful data of withdrawals at Fifer Street
SELECT account_number, amount FROM atm_transactions
    WHERE transaction_type = "withdraw"
    AND atm_location = "Fifer Street"
    AND year = 2020
    AND month = 7
    AND day = 28;

/*
Returns:
A bunch of account numbers. No suspicious amounts withdrawn.
*/

-- Using these account numbers, look for the names attached to them.
SELECT id FROM people
    JOIN bank_accounts ON bank_accounts.person_id = people.id
    JOIN atm_transactions ON atm_transactions.account_number = bank_accounts.account_number
    WHERE transaction_type = "withdraw"
    AND atm_location = "Fifer Street"
    AND year = 2020
    AND month = 7
    AND day = 28;

/*
Returns:
A list of a suspicious thief IDs.
*/

-- Based on Raymond's claim, select phone calls that happened on the date and under a minute.
SELECT caller, receiver FROM phone_calls
    WHERE duration <= 60
    AND year = 2020
    AND month = 7
    AND day = 28;

/*
Returns:
A bunch of phone numbers.
*/

-- Based on the callers, one of these is the thief.
SELECT id FROM people
    JOIN phone_calls ON people.phone_number = phone_calls.caller
    WHERE duration <= 60
    AND year = 2020
    AND month = 7
    AND day = 28;

/*
Returns:
A bunch of suspicious thief IDs.
*/

-- Based on Raymond's claim, select the earliest flight out of Fiftyville the next day.

SELECT * FROM flights
    WHERE year = 2020
    AND month = 7
    AND day = 29
    ORDER BY hour, minute DESC
    LIMIT 1;

/*
Returns:
id | origin_airport_id | destination_airport_id | year | month | day | hour | minute
36 | 8 | 4 | 2020 | 7 | 29 | 8 | 20
*/

-- Look for the city associated with this flight
SELECT city FROM airports
    JOIN flights ON airports.id = flights.destination_airport_id
    WHERE year = 2020
    AND month = 7
    AND day = 29
    ORDER BY hour, minute DESC
    LIMIT 1;

/*
Returns:
London
*/

-- Look for the passengers on this flight. One of these is the thief.
SELECT id FROM people
    JOIN passengers ON passengers.passport_number = people.passport_number
    WHERE flight_id IN (
        -- id of flight they are taking
        SELECT id FROM flights
        WHERE year = 2020
        AND month = 7
        AND day = 29
        ORDER BY hour, minute DESC
        LIMIT 1
    );

/*
Returns:
A list of suspicious thief IDs.
*/

-- Look for the common thief ID between all of these, and search for the name associated with them.

-- Based on these license plates, find people's names attached to them.
SELECT name FROM people WHERE id IN (
    SELECT people.id FROM people
        JOIN courthouse_security_logs ON people.license_plate = courthouse_security_logs.license_plate
        WHERE activity = "exit"
        AND year = 2020
        AND month = 7
        AND day = 28
        AND hour = 10
        AND minute >= 15
        AND minute <= 25

    INTERSECT

    -- Using these account numbers, look for the names attached to them.
    SELECT people.id FROM people
        JOIN bank_accounts ON bank_accounts.person_id = people.id
        JOIN atm_transactions ON atm_transactions.account_number = bank_accounts.account_number
        WHERE transaction_type = "withdraw"
        AND atm_location = "Fifer Street"
        AND year = 2020
        AND month = 7
        AND day = 28

    INTERSECT

    -- Based on the callers, one of these is the thief.
    SELECT people.id FROM people
        JOIN phone_calls ON people.phone_number = phone_calls.caller
        WHERE duration <= 60
        AND year = 2020
        AND month = 7
        AND day = 28

    INTERSECT

    -- Look for the passengers on this flight. One of these is the thief.
    SELECT people.id FROM people
        JOIN passengers ON passengers.passport_number = people.passport_number
        WHERE flight_id IN (
            -- id of flight they are taking
            SELECT id FROM flights
            WHERE year = 2020
            AND month = 7
            AND day = 29
            ORDER BY hour, minute DESC
            LIMIT 1
        )
);

/*
Returns:
Ernest
*/

-- Look for the accomplice who this person called.
SELECT name FROM people
    JOIN phone_calls ON people.phone_number = phone_calls.receiver
    WHERE duration <= 60
    AND year = 2020
    AND month = 7
    AND day = 28
    AND caller IN (
        -- phone number of thief
        SELECT phone_number FROM people WHERE id IN
        (
        -- id of thief
        SELECT id FROM people WHERE id IN (
            SELECT people.id FROM people
                JOIN courthouse_security_logs ON people.license_plate = courthouse_security_logs.license_plate
                WHERE activity = "exit"
                AND year = 2020
                AND month = 7
                AND day = 28
                AND hour = 10
                AND minute >= 15
                AND minute <= 25

            INTERSECT

            -- Using these account numbers, look for the names attached to them.
            SELECT people.id FROM people
                JOIN bank_accounts ON bank_accounts.person_id = people.id
                JOIN atm_transactions ON atm_transactions.account_number = bank_accounts.account_number
                WHERE transaction_type = "withdraw"
                AND atm_location = "Fifer Street"
                AND year = 2020
                AND month = 7
                AND day = 28

            INTERSECT

            -- Based on the callers, one of these is the thief.
            SELECT people.id FROM people
                JOIN phone_calls ON people.phone_number = phone_calls.caller
                WHERE duration <= 60
                AND year = 2020
                AND month = 7
                AND day = 28

            INTERSECT

            -- Look for the passengers on this flight. One of these is the thief.
            SELECT people.id FROM people
                JOIN passengers ON passengers.passport_number = people.passport_number
                WHERE flight_id IN (
                    -- id of flight they are taking
                    SELECT id FROM flights
                    WHERE year = 2020
                    AND month = 7
                    AND day = 29
                    ORDER BY hour, minute DESC
                    LIMIT 1
                )
        )
    )
);

/*
Returns:
Berthold
*/