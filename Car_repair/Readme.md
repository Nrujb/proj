# Описание

Предыстория: существует каршеринговая компания с крупным автопарком машин в нескольких городах. Машины совершают тысячи поездок ежедневно. Люди очень любят этот сервис. А основатели довольны метриками. А чтоб так продолжалось и дальше, машины нуждаются в надлежащим и своевременном обслуживании. Своевременный технический осмотр или мелкий ремонт позволяет предотвратить схождение машин с линии ежедневной аренды на долгий период. 

Проблема: Однако, не все так просто. Автопарк очень большой, люди активно изнашивают машины, а иногда совершают поездки в очень агрессивной манере. Как итог, машины в один момент могут выходить из строя в силу определенных поломок или неисправностей. А техническая бригада автопарка не успевает объезжать все машины каждый день, чтобы предотвратить поломки, проведя превентивные меры.

Идея: Главные бригадир ремонта машин обратился в IT отдел и предложил сформировать преоретизированный список обхода машин. Тогда, тех. бригаде не пришлось бы объезжать все подряд машины, а было бы достаточно обойти только те машины, которые вероятнее всего выйдут из строя раньше. Чтобы определить такой список, надо уметь прогнозировать время поломки машин. 

Решение: Как же повезло, что в компании этого автопарка давно уже есть команда data engineer'ов, которая аккуратно собирает все данные по поездкам. А значит, у нас есть исторические данные, в которых есть информация о состоянии машин, которому предшествовали поломки. Более того, в этой команде оказался Data Scientist, которому предстоит решать эту задачу. Роль этого Data Scientist'а будешь играть ты!

# Основные данные

Главное описание машин с информацией о поломках для обучения / прогноза - car_train.csv / car_test.csv. Пояснение столбцов:
```
car_id - идентификатор машины
model / car_type / fuel_type- марка, класс и тип топлива машины
car_rating / riders- общий рейтинг  и общее число поездок к концу 2021-го кода.
year_to_start / year_to_work- года выпуска машины и начала работы в автопарке 
main_city - город пребывания машины. (Москва или Питер)
target_reg - время до поломки.
target_class - класс поломки (всего 9 видов, отсутствует в тесте)
```

# Дополнительные данные

Помимо описания машин, есть немаловажные данные о поездках на этих машинах за период трех месяцев, ремонтных работах и данных водителей. То есть, есть еще три дополнительные таблицы, о них кратко в следующем степе.
```
rides_info.csv - информация про поездки
driver_info.csv - информация про водителей
fix_info.csv - информация про ремонт машин
```
# Описание дополнительных данных

## Информация про поездки - rides_info.csv
Пояснение столбцов:
```
user_id / car_id / ride_id - идентификаторы водителя, машины, поездки соответственно
ride_date / rating - дата поездки и рейтинг, поставленный водителем 
ride_duration / distance / ride_cost -  длительность (время),  пройденное расстояние, стоимость поездки 
speed_avg / speed_max - средняя и максимальная скорости поездки соответственно
stop_times / refueling - количество остановок (паузы) и флаг - была ли дозаправка.
user_ride_quality - оценка манеры вождения в машины водителя, определенная скоринговой ML системой сервиса.
deviation_normal - общий показатель датчиков о состоянии машины, относительно эталонных показателей (нормы). 
```
## Информация про водителей - driver_info.csv
Пояснение столбцов:
```
user_id / age / sex - идентификатор, возраст и пол водителя соответственно
user_rating - общий рейтинг пользователя за все поездки к концу 2021-го года
user_rides - общее количество поездок к концу 2021-го кода 
user_time_accident - число инцидентов (это могли быть аварии/штрафы/эвакуация машины)  
first_ride_date - дата первой поездки
```

## Информация про ремонт машин - fix_info.csv
Пояснение столбцов:
```
worker_id / car_id - идентификатор работника и машины соответсвенно 
work_type / work_duration - тип и длительность (в часах) проводимой работы
destroy_degree - степень износа/поврежденности машины в случае поломки
fix_date - время начала ремонта (время снятия машины с линии)
```