USE basic_status;

CREATE TABLE IF NOT EXISTS `user`(
    id INT AUTO_INCREMENT PRIMARY KEY,
    basic_status VARCHAR(11),
    probability DOUBLE NOT NULL,
    val1 DOUBLE NOT NULL
);

CREATE TABLE IF NOT EXISTS substatus(
    id INT AUTO_INCREMENT PRIMARY KEY,
    basic_status VARCHAR(11) UNIQUE,
    probability DOUBLE NOT NULL,
    val1 DOUBLE NOT NULL,
    val2 DOUBLE NOT NULL,
    val3 DOUBLE NOT NULL,
    val4 DOUBLE NOT NULL,
    val5 DOUBLE NOT NULL
);

CREATE TABLE IF NOT EXISTS armor(
    id INT AUTO_INCREMENT PRIMARY KEY,
    armor_name VARCHAR(32) UNIQUE,
    probability DOUBLE NOT NULL
);

CREATE TABLE IF NOT EXISTS save_equipment(
    part INT PRIMARY KEY,
    hasResult TINYINT NOT NULL,
    armorName VARCHAR(10) UNIQUE,
    basicStatusLines TEXT,
    statusLines TEXT
);

INSERT IGNORE INTO `user`(basic_status, probability, val1) VALUES
    ('HP%', 10, 42.0),
    ('HP', 11, 1300),
    ('攻撃%', 5, 37.0),
    ('攻撃', 8, 230),
    ('防御%', 10, 48.0),
    ('防御', 10, 1300),
    ('会心率', 8, 35.0),
    ('会心ダメージ', 8, 73.0),
    ('速度', 6, 22),
    ('運値', 10, 23.0),
    ('Poop', 14, 666);
    
INSERT IGNORE INTO substatus(basic_status, probability, val1, val2, val3, val4, val5) VALUES
    ('HP%', 10, 7.0, 8.0, 9.0, 10.0, 11.0),
    ('HP', 11, 300, 400, 500, 600, 700),
    ('攻撃%', 5, 7.0, 8.0, 9.0, 10.0, 11.0),
    ('攻撃', 8, 30, 40, 50, 60, 70),
    ('防御%', 10, 8.0, 9.0, 10.0, 11.0, 12.0),
    ('防御', 10, 300, 400, 500, 600, 700),
    ('会心率', 8, 5.0, 6.0, 7.0, 9.0, 11.0),
    ('会心ダメージ', 8, 13.0, 15.0, 17.0, 20.0, 22.0),
    ('速度', 6, 2, 3, 4, 5, 7),
    ('運値', 10, 3.0, 4.0, 5.0, 6.0, 7.0),
    ('Poop', 14, 111, 121, 169, 225, 300);
    
INSERT IGNORE INTO armor(armor_name, probability) VALUES
    ('a', 25),
    ('b', 25),
    ('c', 25),
    ('d', 25);

SELECT * FROM `user`;
SELECT * FROM substatus;
SELECT * FROM armor;
SELECT * FROM save_equipment;

commit;



