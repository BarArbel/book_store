#include "db.h"

Database *Database::instance = 0;

Database::Database() : driver(get_driver_instance()) {
	connection_properties["hostName"] = DB_HOST;
	connection_properties["port"] = DB_PORT;
	connection_properties["userName"] = DB_USER;
	connection_properties["password"] = DB_PASS;
	connection_properties["OPT_RECONNECT"] = true;

	// use database, create tables and insert values.
	try {
		Connection *con = driver->connect(connection_properties);
		try {
			con->setSchema("book_store");
		}
		catch (SQLException &e) {
			Statement *stmt = con->createStatement();
			stmt->execute("CREATE DATABASE IF NOT EXISTS book_store;");
						 // "USE book_store;");
			con->setSchema("book_store"); // switch database
			stmt->execute("CREATE TABLE IF NOT EXISTS books("
						  "book_id INT UNSIGNED NOT NULL PRIMARY KEY,"
						  "title VARCHAR(100),"
						  "author_name VARCHAR(100)"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS genres("
						  "genre_id INT UNSIGNED NOT NULL PRIMARY KEY,"
						  "g_name VARCHAR(100)"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS store_items("
						  "book_id INT UNSIGNED NOT NULL,"
						  "genre_id INT UNSIGNED NOT NULL,"
						  "FOREIGN KEY(book_id) REFERENCES books(book_id),"
						  "FOREIGN KEY(genre_id) REFERENCES genres(genre_id),"
						  "price DOUBLE,"
						  "discount INT UNSIGNED /* % */"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS inventory("
						  "book_id INT UNSIGNED,"
						  "FOREIGN KEY(book_id) REFERENCES store_items(book_id),"
						  "amount INT UNSIGNED"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS suppliers("
						  "supp_id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,"
						  "supp_name VARCHAR(100),"
						  "phone VARCHAR(10),"
						  "bank_acc VARCHAR(10)"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS book_prices("
						  "book_id INT UNSIGNED,"
						  "supp_id INT UNSIGNED,"
						  "FOREIGN KEY(supp_id) REFERENCES suppliers(supp_id),"
						  "FOREIGN KEY(book_id) REFERENCES store_items(book_id),"
						  "PRIMARY KEY(supp_id, book_id), /* check if this causes problems */"
						  "price DOUBLE"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS customers("
						  "cust_id INT UNSIGNED NOT NULL PRIMARY KEY,"
						  "first_name VARCHAR(100),"
						  "last_name VARCHAR(100),"
						  "phone VARCHAR(10)"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS sellers("
						 "seller_id INT UNSIGNED NOT NULL PRIMARY KEY,"
						 "first_name VARCHAR(100),"
						 "last_name VARCHAR(100)"
						 ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS purchases("
						  "purch_id INT UNSIGNED NOT NULL PRIMARY KEY,"
						  "book_id INT UNSIGNED,"
						  "seller_id INT UNSIGNED,"
						  "cust_id INT UNSIGNED,"
						  "FOREIGN KEY(book_id) REFERENCES store_items(book_id),"
						  "FOREIGN KEY(seller_id) REFERENCES sellers(seller_id),"
						  "FOREIGN KEY(cust_id) REFERENCES customers(cust_id),"
						  "purch_date DATE,"
						  "canceled BOOL,"
						  "origin_price DOUBLE,"
						  "cust_pay DOUBLE"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS order_status("
						  "status_id INT UNSIGNED NOT NULL PRIMARY KEY,"
						  "stat_type VARCHAR(100)"
						  ");");
			stmt->execute("CREATE TABLE IF NOT EXISTS orders("
						  "order_id INT UNSIGNED NOT NULL PRIMARY KEY,"
						  "book_id INT UNSIGNED,"
						  "supp_id INT UNSIGNED,"
						  "cust_id INT UNSIGNED,"
						  "status_id INT UNSIGNED,"
						  "FOREIGN KEY(book_id) REFERENCES store_items(book_id),"
						  "FOREIGN KEY(supp_id) REFERENCES suppliers(supp_id),"
						  "FOREIGN KEY(cust_id) REFERENCES customers(cust_id),"
						  "amount INT UNSIGNED,"
						  "/*status ENUM ('not ordered yet','ordered','received','message sent to customer','sold','canceled'),*/"
						  "FOREIGN KEY(status_id) REFERENCES order_status(status_id),"
						  "order_date DATE,"
						  "tot_price DOUBLE"
						  ");");

			stmt->execute("INSERT INTO books (book_id, title, author_name) "
						  "VALUES "
						  "(1, 'The Hitchhikers Guide to the Galaxy', 'Douglas Adams'), "
						  "(2, 'The Lord of the Rings', 'J. R. R. Tolkien'), "
						  "(3, 'The Prince of Milk', 'Exurb1a'), "
						  "(4, 'The Subtle Art of Not Cooking a Fudge', 'Mark Manson'), "
						  "(5, 'A Brief History of Time', 'Stephen Hawking'), "
						  "(6, 'Parallel Worlds', 'Michio Kaku'), "
						  "(7, 'Angels & Demons', 'Dan Brown'), "
						  "(8, 'The End of Mr. Y', 'Scarlett Thomas'), "
						  "(9, '1984', 'George Orwell'), "
						  "(10, 'American Gods', 'Neil Gaiman'), "
						  "(11, 'The Nerdy Nummies Cookbook', 'Rosanna Pansino'), "
						  "(12, 'Norse Mythology', 'Neil Gaiman'), "
						  "(13, 'The Universe in a Nutshell', 'Stephen Hawking');");
			stmt->execute("INSERT INTO genres (genre_id, g_name) "
						  "VALUES "
						  "(1, 'Sci-fi'), "
						  "(2, 'Fantasy'), "
						  "(3, 'Cooking'), "
						  "(4, 'Popular Science'), "
						  "(5, 'Novel'), "
						  "(6, 'Political Fiction'), "
						  "(7, 'DIY'), "
						  "(8, 'Noir Fiction'), "
						  "(9, 'Astronomy'), "
						  "(10, 'Poetry');");
			stmt->execute("INSERT INTO store_items (book_id, genre_id, price, discount) "
						  "VALUES "
						  "(1, 1, 40, 0), "
						  "(2, 2, 70, 20), "
						  "(3, 1, 40, 0), "
						  "(4, 3, 1200, 10), "
						  "(5, 4, 100, 0), "
						  "(6, 4, 100, 0), "
						  "(7, 5, 70, 20), "
						  "(8, 5, 70, 0), "
						  "(9, 6, 70, 0), "
						  "(10, 2, 70, 20), "
						  "(11, 3, 40, 0), "
						  "(12, 2, 75, 0), "
						  "(13, 9, 100, 20);");
			stmt->execute("INSERT INTO inventory (book_id, amount) "
						  "VALUES "
						  "(1, 0), "
						  "(2, 61), "
						  "(3, 20), "
						  "(4, 14), "
						  "(5, 78), "
						  "(6, 20), "
						  "(7, 3), "
						  "(8, 5), "
						  "(9, 34), "
						  "(10, 0), "
						  "(11, 53), "
						  "(12, 0), "
						  "(13, 20);");
			stmt->execute("INSERT INTO suppliers (supp_id, supp_name, phone, bank_acc) "
						  "VALUES "
						  "(1, 'Booky', '0318294729', '4820182389'), "
						  "(2, 'Shelfie', '0384882373', '1029384756'), "
						  "(3, 'B00k5', '0322294729', '1029323756'), "
						  "(4, 'Sefer Tov', '0323652839', '1029323753'), "
						  "(5, 'eBookie Kabookie', '0318291659', '0820182389'), "
						  "(6, 'Meow', '0392933477', '1234888840'), "
						  "(7, 'Moo', '0393333377', '1234848840'), "
						  "(8, 'G00D Book', '0393563377', '1234748840'), "
						  "(9, 'Look4Book', '0391276438', '1200492048'), "
						  "(10, 'Sfarimim', '0301276438', '1200491048');");
			stmt->execute("INSERT INTO book_prices (book_id, supp_id, price) "
						  "VALUES "
						  "(1, 1, 20), "
						  "(1, 5, 20), "
						  "(1, 3, 20), "
						  "(2, 7, 35), "
						  "(2, 3, 35), "
						  "(2, 2, 35), "
						  "(2, 8, 30), "
						  "(2, 9, 35), "
						  "(3, 6, 20), "
						  "(3, 7, 20), "
						  "(3, 8, 20), "
						  "(3, 9, 20), "
						  "(4, 10, 600), "
						  "(5, 10, 50), "
						  "(6, 3, 50), "
						  "(6, 4, 50), "
						  "(6, 5, 30), "
						  "(6, 6, 45), "
						  "(7, 1, 35), "
						  "(7, 2, 35), "
						  "(7, 3, 35), "
						  "(7, 4, 30), "
						  "(7, 5, 35), "
						  "(7, 8, 20), "
						  "(8, 6, 20), "
						  "(9, 7, 35), "
						  "(9, 9, 35), "
						  "(10, 7, 35), "
						  "(10, 8, 30), "
						  "(11, 8, 20), "
						  "(12, 9, 30), "
						  "(12, 10, 30), "
						  "(12, 5, 25), "
						  "(12, 2, 25), "
						  "(13, 6, 50);");
			stmt->execute("INSERT INTO customers (cust_id,first_name,last_name,phone) "
						  "VALUES "
						  "(1, 'Sans', 'Comic', '0532857390'), "
						  "(2, 'Yad', 'Guttman', '0538772293'), "
						  "(3, 'David', 'Arial', '0541188374'), "
						  "(4, 'Champ', 'Underwood', '0558384247'), "
						  "(5, 'David', 'Arial', '0534266667'), "
						  "(6, 'David', 'Arial', '0529827423'), "
						  "(7, 'Cookie', 'Cutter', '0544839211'), "
						  "(8, 'Charles', 'Wright', '0562582744'), "
						  "(9, 'Tahoma', 'Bold', '0534266666'), "
						  "(10, 'Wing', 'Ding', '0534266662');");
			stmt->execute("INSERT INTO sellers (seller_id, first_name, last_name) "
						  "VALUES "
						  "(1, 'Monika', 'Yuri'), "
						  "(2, 'Natsuki', 'Sayori'), "
						  "(3, 'Dan', 'Salvato'), "
						  "(4, 'Satchely', 'Velinquent');");
			stmt->execute("INSERT INTO purchases (purch_id, book_id, seller_id, cust_id, purch_date, canceled, origin_price, cust_pay) "
						  "VALUES "
						  "(1, 2, 1, 1, '2016-12-21', false, 70, 56), "
						  "(2, 10, 2, 7, '2016-12-28', false, 70, 56), "
						  "(3, 2, 3, 9, '2017-01-05', false, 70, 56), "
						  "(4, 3, 4, 8, '2017-01-15', false, 40, 40), "
						  "(5, 2, 1, 10, '2017-01-25', true, 70, 56), "
						  "(6, 11, 2, 4, '2017-02-27', true, 40, 40), "
						  "(7, 7, 3, 8, '2017-03-04', false, 70, 56), "
						  "(8, 11, 4, 9, '2017-03-22', false, 40, 40), "
						  "(9, 10, 4, 4, '2017-03-22', false, 70, 56), "
						  "(10, 4, 4, 2, '2017-03-30', false, 1200, 1080), "
						  "(11, 2, 1, 3, '2017-04-17', false, 70, 56), "
						  "(12, 10, 2, 2, '2017-05-17', false, 70, 50.4), "
						  "(13, 6, 3, 3, '2017-06-17', false, 100, 100), "
						  "(14, 5, 4, 4, '2017-06-18', false, 100, 100), "
						  "(15, 13, 1, 2, '2017-06-18', true, 100, 72), "
						  "(16, 2, 2, 6, '2017-06-18', false, 70, 56), "
						  "(17, 9, 3, 4, '2017-07-04', false, 70, 70), "
						  "(18, 8, 4, 6, '2017-08-04', false, 70, 70), "
						  "(19, 10, 4, 4, '2017-09-23', false, 70, 56), "
						  "(20, 4, 4, 5, '2017-10-14', false, 1200, 1080), "
						  "(21, 2, 1, 1, '2017-11-14', false, 70, 56), "
						  "(22, 6, 2, 7, '2017-11-21', false, 100, 100), "
						  "(23, 2, 3, 9, '2017-11-28', false, 70, 56), "
						  "(24, 3, 4, 8, '2017-12-01', false, 40, 40), "
						  "(25, 2, 1, 4, '2017-12-05', false, 70, 56), "
						  "(26, 11, 2, 4, '2017-12-10', false, 40, 40), "
						  "(27, 4, 3, 4, '2017-12-10', false, 1200, 1080), "
						  "(28, 1, 4, 9, '2018-01-28', false, 40, 40), "
						  "(29, 10, 4, 4, '2018-02-14', false, 70, 56), "
						  "(30, 12, 4, 2, '2018-02-14', false, 75, 75);");
			stmt->execute("INSERT INTO order_status (status_id, stat_type) "
						  "VALUES "
						  "(1, 'Not ordered yet'), "
						  "(2, 'Ordered'), "
						  "(3, 'Received'), "
						  "(4, 'Message sent to customer'), "
						  "(5, 'Sold'), "
						  "(6, 'Canceled');");
			stmt->execute("INSERT INTO orders (order_id, book_id, supp_id, cust_id, status_id, amount, order_date, tot_price) "
						  "VALUES "
						  "(1, 2, 8, NULL, 3, 50, '2016-12-22', 1500), "
						  "(2, 3, 8, 8, 5, 1, '2017-01-10', 40), "
						  "(3, 5, 10, 7, 6, 1, '2017-03-18', 50), "
						  "(4, 8, 6, 6, 5, 1, '2017-08-01', 20), "
						  "(5, 8, 6, NULL, 3, 5, '2017-08-05', 100), "
						  "(6, 6, 5, 7, 5, 1, '2017-11-15', 30), "
						  "(7, 6, 5, NULL, 3, 20, '2017-11-22', 600), "
						  "(8, 3, 8, 8, 5, 1, '2017-11-27', 40), "
						  "(9, 3, 8, NULL, 2, 20, '2017-12-01', 800), "
						  "(10, 12, 5, 3, 1, 1, '2018-02-15', 25);");
			delete stmt;
		}
		delete con;
	}
	catch (SQLException &e) {
		cout << e.getErrorCode() << " " << e.what() << " " << e.getSQLState();
	}
}

Database & Database::getInstance() {
	if (Database::instance == 0) {
		instance = new Database();
	}
	return *instance;
}

Connection * Database::getConnection() {
	try {
		Connection *con = driver->connect(connection_properties);
		con->setSchema(DB_NAME);
		return con;
	}
	catch (SQLException &e) {
		cout << e.what();
	}
	return 0;
}
