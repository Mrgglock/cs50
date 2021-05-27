/* Question:
Select Kevin Bacon in 1958
Search for people who starred with him
*/

/* Method:
Get ID of Kevin Bacon (1958)
Get movies that this Person ID starred in
Get all People ID from these movies
Run Distinct on it
Convert to names
Exclude Kevin Bacon Himself
*/

-- Execution

-- Convert people IDs to people names
SELECT name FROM people WHERE id IN (
    -- DISTINCT People IDs,
    SELECT DISTINCT person_id FROM stars
    -- 1) Excluding Kevin Bacon
    WHERE person_id NOT IN (
        -- ID of Kevin Bacon (1958)
        SELECT id from people
            WHERE name = "Kevin Bacon"
            AND birth = 1958
    )
    -- 2) which starred in the following movie IDs
    AND movie_id IN (
        -- Movie IDs that this Kevin Bacon (1958) starred in
        SELECT movie_id FROM stars
        WHERE person_id IN (
            -- ID of Kevin Bacon (1958)
            SELECT id from people
                WHERE name = "Kevin Bacon"
                AND birth = 1958
        )
    )
);