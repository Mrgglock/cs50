SELECT name FROM people WHERE people.id IN 
(
    SELECT DISTINCT id FROM people
    JOIN directors ON people.id = directors.person_id
    JOIN ratings ON directors.movie_id = ratings.movie_id
    WHERE rating >= 9.0
);