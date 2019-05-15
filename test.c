#include <my_global.h>
#include <mysql.h>


void finish_with_error(MYSQL *mysql)
{
	fprintf(stderr,"%s\n",mysql_error(mysql));
	mysql_close(mysql);
	exit(1);
}

int main(int argc, char** argv)
{
	MYSQL* mysql=mysql_init(NULL);
	if(mysql ==NULL)
	{
		fprintf(stderr,"%s\n",mysql_error(mysql));
		exit(1);
	}
	
	if(mysql_real_connect(mysql,"localhost","jae","2068","testdb",0,NULL,0)==NULL)
	{
		finish_with_error(mysql);
	}

	if(mysql_query(mysql,"DROP TABLE IF EXISTS testdb"))
	{
		finish_with_error(mysql);
	}
	if(mysql_query(mysql, "CREATE TABLE testdb(Id INT, Name TEXT, Price INT)"))
	{
		finish_with_error(mysql);
	}
	
	
	if(mysql_query(mysql, "INSERT INTO testdb VALUES(1,'Hyundai',8000)"))
	{
		finish_with_error(mysql);
	}
	if(mysql_query(mysql, "INSERT INTO testdb VALUES(1,'Audi',9000)"))
	{
		finish_with_error(mysql);
	}
	if(mysql_query(mysql, "INSERT INTO testdb VALUES(1,'GM',7000)"))
	{
		finish_with_error(mysql);
	}
	
	mysql_close(mysql);

}
