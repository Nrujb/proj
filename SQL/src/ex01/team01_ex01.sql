-- insert into currency values (100, 'EUR', 0.85, '2022-01-01 13:29');
-- insert into currency values (100, 'EUR', 0.79, '2022-01-08 13:29');

SELECT
    name,
    lastname,
    currency_name,
    money * rate_to_usd AS currency_in_usd
FROM (
         SELECT
             COALESCE("user".name, 'not defined') AS name,
             COALESCE("user".lastname, 'not defined') AS lastname,
             currency.name AS currency_name,
             balance.money,
             COALESCE((select rate_to_usd
                       from currency
                       where balance.currency_id = currency.id and currency.updated < balance.updated
                       order by currency.updated desc
                      limit 1),
				(select rate_to_usd
				from currency
				where balance.currency_id = currency.id and currency.updated > balance.updated
				order by currency.updated asc
				limit 1
				)) AS rate_to_usd
         FROM balance
         INNER JOIN (select currency.id, currency.name from currency group by currency.id, currency.name) as currency
            ON currency.id = balance.currency_id
         LEFT JOIN "user"
            ON balance.user_id = "user".id
     )
ORDER BY 1 desc, 2, 3