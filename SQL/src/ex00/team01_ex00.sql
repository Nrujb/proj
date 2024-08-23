SELECT distinct
    name,
    lastname,
    type,
    sum(money::numeric) as volume,
    currency_name,
    last_rate_to_usd,
    (sum(money) * last_rate_to_usd) as total_volume_in_usd
FROM (
         SELECT distinct
             COALESCE("user".name, 'not defined') as name,
             COALESCE("user".lastname, 'not defined') as lastname,
             balance.type,
             balance.money,
             COALESCE(currency.name, 'not defined') as currency_name,
             COALESCE(first_value(currency.rate_to_usd) over(partition by currency.id order by currency.updated desc), 1) as last_rate_to_usd
         FROM balance
         full JOIN "user"
            ON balance.user_id = "user".id
         full JOIN currency
            ON balance.currency_id = currency.id
     )
GROUP BY name, lastname, type, currency_name, last_rate_to_usd
ORDER BY name desc, lastname, type