#include <iostream>
#include <string>
#include <string.h>
#include "C:\Program Files\PostgreSQL\9.4\include\libpq-fe.h"
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ESC 27
#define KEY_ENTER 13
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <sstream>
#define SSTR( x ) dynamic_cast< ostringstream & >( ( ostringstream() << dec << x ) ).str()
using namespace std;



//funcoes auxiliares
int is_numeric(const char *str);
int valida_cpf(string str);
int valida_data(int dia, int mes, int ano);
void toUpString(string &str);
string getParamName(string parameter);
string getParamValue(string value);
void helpOptions();
void sair();
void pause (float);
//funcoes atuante no banco
int menu(PGconn *conn);

//Consulta
int menu_consulta(PGconn *conn);
void consulta_Tabelas(int numero, PGconn* conn);
int consulta(PGconn *conn, const char *con);

//Insercao
int menu_inserir(PGconn *conn);
int inserir(PGconn *conn, const char *con);

//Apagar
int menu_apagar(PGconn *conn);
void apaga_Tabelas(int numero, PGconn* conn);
int apaga(PGconn *conn, const char *con);

//Atualizar
int menu_atualizar(PGconn *conn);
int atualiza(PGconn *conn, const char *con);






int main(int argc, char **argv)
{
	string conninfo = "";
	PGconn *conn = NULL; // Usado para a conexao com o Postgres
	string parametrosCMD;
	for (short i = 1; i < argc; i++)
	{
		parametrosCMD = getParamName(argv[i]);
		if(parametrosCMD == "host")
		{
			conninfo = conninfo + " host="+getParamValue(argv[i]);
		}
		else if(parametrosCMD == "port")
		{
			conninfo = conninfo + " port="+getParamValue(argv[i]);
		}
		else if(parametrosCMD == "dbname")
		{
			conninfo = conninfo +" dbname="+getParamValue(argv[i]);
		}
		else if(parametrosCMD == "user")
		{
			conninfo = conninfo +" user="+getParamValue(argv[i]);
		}
		else if(parametrosCMD == "password")
		{
			conninfo = conninfo +" password="+getParamValue(argv[i]);
		}
		else if(parametrosCMD == "help")
		{
			helpOptions();
			return 1;
		}
	}
	if (argc == 1)
	{
		cout << "Parametros nao encontrados!!!" << endl << endl;
		helpOptions();
		return 1;
	}
	//PQconnectdb retorna null se houver falha na alocação dinamica de memoria
	conn=PQconnectdb(conninfo.c_str());//c_str converte a string para vetor de char
	// Verifica se conectou corretamente
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		//PQstatus retorna connection_bad ou conection_ok, conexao falhou ou ok respectivamente
		cout << PQerrorMessage(conn);
		PQreset(conn); //encerra a conexao com o banco
		return 1;
	}
	else
		cout << "Conexao executada com sucesso!" << endl;
	pause(1);
	menu(conn);
	return 0;
}


int menu(PGconn *conn)
{
    int key=0,cont=1;
    while(1)
    {
        system("cls");
        printf("%c========Menu Principal===========%c\n",201,187);
        printf("%c %s Consultar no Banco            %c\n", 186,(cont == 1)? "X": " ",186);
        printf("%c %s Inserir no Banco              %c\n",186,(cont == 2) ? "X": " ",186);
        printf("%c %s Apagar no Banco               %c\n",186, (cont == 3)? "X": " ",186);
        printf("%c %s Atualizar o Banco             %c\n",186, (cont == 4)? "X": " ",186);
        printf("%c %s Sair                          %c\n",186, (cont == 5)? "X": " ",186);
        printf("%c                                 %c\n",186,186);
        printf("%c=================================%c\n",200,188);
        key = getch();
        if (key == KEY_ESC)
            return 0;
        else if(key == KEY_UP)
        {
            if(cont==0)
                cont++;
            else if(cont>=1 && cont<=5)
                cont--;
        }
        else if(key == KEY_DOWN)
        {
        	if(cont>=0 && cont<=4)
                cont++;
            else if(cont==5)
                cont--;
        }
        else if(key==KEY_ENTER)
        {
            if(cont==1)
            {
				menu_consulta(conn);
				PQreset(conn);
            }
            else if(cont==2)
            {
            	menu_inserir(conn);
            	PQreset(conn);
            }
            else if(cont==3)
            {
            	menu_apagar(conn);
            	PQreset(conn);
            }
            else if(cont==4)
            {	
				menu_atualizar(conn);
				PQreset(conn);
            }
            else if(cont==5)
            {
				//Fecha a conexão com o banco
				PQfinish(conn);
                sair();
            }
        }
    }
}


int menu_consulta(PGconn *conn)
{
	int key=0,cont=1;
	    while(1)
	    {
	        system("cls");
	        printf("%c===========================Selecione uma Consulta============================%c\n",201,187);
	        printf("%c %s Uma Tabela inteira                                                        %c\n", 186,(cont == 1)? "X": " ",186);
	        printf("%c %s Todas as compras feitas por um cliente atraves de seu nome                %c\n",186,(cont == 2) ? "X": " ",186);
	        printf("%c %s Todos clientes que um funcionario atendeu                                 %c\n",186, (cont == 3)? "X": " ",186);
	        printf("%c %s Nome, codigo de barras, total e nro parcelas dos que pagaram via boleto   %c\n",186, (cont == 4)? "X": " ",186);
	        printf("%c %s Nome, numero, vencimento, total e nro parcelas dos que pagaram via cartao %c\n",186, (cont == 5)? "X": " ",186);
	        printf("%c %s Nome, desconto, total e nro parcelas dos que pagaram via carne            %c\n",186, (cont == 6)? "X": " ",186);
	        printf("%c %s Selecionar todos funcionarios que nao possuem familiares                  %c\n",186, (cont == 7)? "X": " ",186);
	        printf("%c %s Nome, cpf e contagem dos clientes que ordenaram mais que 2 pedidos        %c\n",186, (cont == 8)? "X": " ",186);
	        printf("%c %s Nome de todos os clientes que pediram todas as pecas que o cliente X pediu%c\n",186, (cont == 9)? "X": " ",186);
	        printf("%c %s A media das idade de todos os clientes do sexo masculino                  %c\n",186, (cont == 10)? "X": " ",186);
	        printf("%c %s Todos clientes que pediram algo da marca Golden                           %c\n",186, (cont == 11)? "X": " ",186);
	        printf("%c %s Voltar ao Menu Principal                                                  %c\n",186, (cont == 12)? "X": " ",186);
	        printf("%c %s Sair                                                                      %c\n",186,(cont == 13) ? "X": " ",186);
	        printf("%c                                                                             %c\n",186,186);
	        printf("%c=============================================================================%c\n",200,188);
	        key = getch();
	        if (key == KEY_ESC)
	            return 0;
	        else if(key == KEY_UP)
	        {
	            if(cont==0)
	                cont++;
	            else if(cont>=1 && cont<=13)
	                cont--;
	        }
	        else if(key == KEY_DOWN)
	        {
	            if(cont>=0 && cont<=12)
	                cont++;
	            else if(cont==13)
	                cont--;
	        }
	        else if(key==KEY_ENTER)
	        {
	            if(cont==1)
	            {
	            	int numero;
	            	do
	            	{
	            		system("cls");
	            		cout<<"Digite uma opcao: \n";
						cout<<"1 - Cliente"<<endl;
						cout<<"2 - Funcionario"<<endl;
						cout<<"3 - Peca"<<endl;
						cout<<"4 - Pagamento"<<endl;
						cout<<"5 - Familiar"<<endl;
						cout<<"6 - Boleto"<<endl;
						cout<<"7 - Cartao"<<endl;
						cout<<"8 - Carne"<<endl;
						cout<<"9 - Registra_pedido"<<endl;
						cout<<"10 - Voltar ao menu anterior"<<endl;
						cin>>numero;
						if(numero>=1 && numero<=9)
							consulta_Tabelas(numero,conn);
						else if(numero==10)
						{
							menu_consulta(conn);
						}
	            	}
					while(numero<1 || numero>10);
	            }
	            else if(cont==2)
	            {
	            	string nome,query="";
	            	cout<<"Insira o nome do cliente: ";
	            	cin>>nome;
	            	query = "select Pnome, numero, nome"
	            			" from cliente,registra_pedido,peca"
	            			" where cpf_cliente=cpf and pnome=\'"+nome+
							"\' and numero = num_peca;";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==3)
	            {
	            	string nome,query="";
	            	cout<<"Insira o nome do funcionario: ";
	            	cin>>nome;
	            	query = "select distinct Cliente.Pnome, Cliente.Unome"
	            	" from cliente,registra_pedido,funcionario"
	           	    " where Funcionario.Pnome=\'"+nome+"\' and codigo_funcionario=codigo"
	           	    " and cpf_cliente=cpf;";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==4)
	            {
	            	string nome,query="";
	            	query = "select pnome,unome,codigo_barras,sum(preco) as total, parcelas"
	            			" from cliente,registra_pedido,peca,pagamento,boleto"
	            			" where registra_pedido.cpf_cliente=cpf and numero=num_peca"
	            			" and cpf=pagamento.cpf_cliente and boleto.cpf_cliente=cpf"
	            			" group by pnome,unome,codigo_barras,parcelas"
	            			" order by pnome;";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==5)
	            {
	            	string nome,query="";
	            	query = "select pnome,unome,Cartao.numero,vencimento,sum(preco) as total, parcelas"
	            			" from cliente,registra_pedido,peca,pagamento,cartao"
	            			" where registra_pedido.cpf_cliente=cpf and peca.numero=num_peca"
	            			" and cpf=pagamento.cpf_cliente and cartao.cpf_cliente=cpf"
	            			" group by pnome,unome,Cartao.numero,vencimento,parcelas"
	            			" order by pnome;";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==6)
	            {
	            	string nome,query="";
	            	query = "select pnome,unome,desconto,sum(preco) as total, parcelas"
	            		    " from cliente,registra_pedido,peca,pagamento,carne"
	            			" where registra_pedido.cpf_cliente=cpf and peca.numero=num_peca"
	            			" and cpf=pagamento.cpf_cliente and carne.cpf_cliente=cpf"
	            			" group by pnome,unome,desconto,parcelas"
	            			" order by pnome;";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==7)
	            {
	            	string nome,query="";
	            	query = "select pnome,unome"
	            			" from funcionario"
	            			" where not exists"
	            			" (select *"
	            			" from familiar"
	            			" where codigo=codigo_funcionario"
	            			");";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==8)
	            {
	            	string nome,query="";
	            	query = "select pnome, unome, cpf, count(*) "
	            			" from registra_pedido,cliente"
	            			" where cpf=cpf_cliente"
	            			" group by pnome,unome,cpf"
	            			" having count(*)>2"
	            			" order by pnome;";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==9)
	            {
	            	string nome,query="";
	            	cout<<"Insira o nome do cliente X: ";
	            	cin>>nome;
	            	query = "select cpf,pnome,unome"
	            			" from cliente"
	            			" where not exists"
	            			" (	(select num_peca"
	            			"    from registra_pedido,cliente"
	            			"     where pnome='"+nome+"' and cpf_cliente=cpf"
							" 	)"
							"   except"
							"  	( select num_peca"
							"  	  from registra_pedido"
							"  	 where cpf=cpf_cliente"
							"    )"
							")";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==10)
	            {
	            	string nome,query="";
	            	query = "select avg(extract(year from age(data_nascimento))) as Media_idades"
	            			" from cliente"
	            			" where sexo='M'";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==11)
	            {
	            	string nome,query="";
	            	query = "select pnome, unome,nome, tipo,preco"
	            			" from cliente,registra_pedido,peca"
	            			" where nome like '%Golden' and cpf_cliente=cpf and numero=num_peca";
	            	consulta(conn,query.c_str());
	            }
	            else if(cont==12)
	            {
	            	menu(conn);
	            }
	            else if(cont==13)
	            {
	            	//Fecha a conexão com o banco
	            	PQfinish(conn);
	            	sair();
	           }
	        }
	    }
}

void consulta_Tabelas(int numero, PGconn* conn)
{

	char * query = new char [500];
	switch(numero)
	{
		case 1:
			strcpy(query,"select * from cliente");
			consulta(conn, query);
			break;
		case 2:
			strcpy(query,"select * from funcionario");
			consulta(conn, query);
			break;
		case 3:
			strcpy(query,"select * from peca");
			consulta(conn, query);
			break;
		case 4:
			strcpy(query,"select * from pagamento");
			consulta(conn, query);
			break;
		case 5:
			strcpy(query,"select * from familiar");
			consulta(conn, query);
			break;
		case 6:
			strcpy(query,"select * from boleto");
			consulta(conn, query);
			break;
		case 7:
			strcpy(query,"select * from cartao");
			consulta(conn, query);
			break;
		case 8:
			strcpy(query,"select * from carne");
			consulta(conn, query);
			break;
		case 9:
			strcpy(query,"select * from registra_pedido");
			consulta(conn, query);
			break;
	}
}

int consulta(PGconn *conn, const char *con)
{
	int numLinhas,numColunas;
	string nova;
	PGresult *result; // PGresult armazenará o resultado de uma consulta
	char * consulta = new char [500];
	strcpy(consulta,con);
	result = PQexec(conn, consulta);
	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		cout << PQerrorMessage(conn);
		PQclear(result); // libera os dados armazenados
		PQreset(conn); // reseta a conexao
		return 0;
	}
	else
		cout<<"Sua consulta foi realizada com sucesso!\n"<<endl;
	numLinhas = PQntuples(result);
	// Obtém o número de colunas ou atributos
	numColunas = PQnfields(result);
	//Imprimindo o nome dos atributos da tabela
	if(numLinhas==0)
		cout<<"Porem nao possui resultado!\n";
	else
		for (int j = 0; j < numColunas; j++)
		{
			printf("%-17s", PQfname(result,j));
		}
	cout << endl;
	// Percorre todos os registros
	for (int i = 0; i < numLinhas; i++)
	{
	// Percorre todas as colunas
		for (int j = 0; j < numColunas; j++)
		{
			// Imprime o valor do campo
			printf("%-17s", PQgetvalue(result, i, j));
		}
		cout << endl;
	}
	// Fecha o acesso aos registros
	PQclear(result);
	system("pause");
	return 1;
}

int menu_inserir(PGconn *conn)
{
	int key=0,cont=1;
		    while(1)
		    {
		        system("cls");
		        printf("%c=============================Inserir em uma Tabela===========================%c\n",201,187);
		        printf("%c %s Cliente                                                                   %c\n", 186,(cont == 1)? "X": " ",186);
		        printf("%c %s Funcionario                                                               %c\n",186,(cont == 2) ? "X": " ",186);
		        printf("%c %s Peca                                                                      %c\n",186, (cont == 3)? "X": " ",186);
		        printf("%c %s Pagamento                                                                 %c\n",186, (cont == 4)? "X": " ",186);
		        printf("%c %s Familiar                                                                  %c\n",186, (cont == 5)? "X": " ",186);
		        printf("%c %s Boleto                                                                    %c\n",186, (cont == 6)? "X": " ",186);
		        printf("%c %s Cartao                                                                    %c\n",186, (cont == 7)? "X": " ",186);
		        printf("%c %s Carne                                                                     %c\n",186, (cont == 8)? "X": " ",186);
		        printf("%c %s Registra_Pedido                                                           %c\n",186, (cont == 9)? "X": " ",186);
		        printf("%c %s Voltar ao Menu Principal                                                  %c\n",186, (cont == 10)? "X": " ",186);
		        printf("%c %s Sair                                                                      %c\n",186,(cont == 11) ? "X": " ",186);
		        printf("%c                                                                             %c\n",186,186);
		        printf("%c=============================================================================%c\n",200,188);
		        key = getch();
		        if (key == KEY_ESC)
		            return 0;
		        else if(key == KEY_UP)
		        {
		            if(cont==0)
		                cont++;
		            else if(cont>=1 && cont<=11)
		                cont--;
		        }
		        else if(key == KEY_DOWN)
		        {
		            if(cont>=0 && cont<=10)
		                cont++;
		            else if(cont==11)
		                cont--;
		        }
		        else if(key==KEY_ENTER)
		        {
		            if(cont==1)
		            {
		            	int dia,mes,ano,ok=1;
		            	string cpf="",pnome="",unome="",data="",sexo="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf (11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	cout<<"Insira o valor do primeiro nome: ";
		            	cin>>pnome;
		            	cout<<"Insira o valor do ultimo nome: ";
		            	cin>>unome;
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o sexo (M/F): ";
		            		cin>>sexo;
		            		if(sexo=="M" || sexo=="m" || sexo=="F" || sexo=="f")
		            		{
		            			ok=1;
		            			toUpString(sexo);
		            		}
		            		else
		            		{
		            			ok=0;
		            			cout<<"Sexo invalido, tente novamente"<<endl;
		            		}
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira a data de nascimento (dd-mm-aaaa): ";
		            		scanf("%d-%d-%d",&dia,&mes,&ano);
		            		fflush(stdin);
		            		if(valida_data(dia,mes,ano)!=1)
		            		{
		            			ok=0;
		            			cout<<"Data invalida"<<endl;
		            		}
		            		else
		            		{

		            			data = SSTR(dia)+"-"+SSTR(mes)+"-"+SSTR(ano);
		            			if(ok==1)
		            				ok=1;
		            		}
		            	}while(ok!=1);
		            	query="insert into cliente(cpf,pnome,unome,data_nascimento,sexo) "
		            		"values ('"+cpf+"','"+pnome+"','"+unome+"','"+data+"','"+sexo+"');";
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	cpf="",pnome="",unome="",data="",sexo="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	dia=mes=ano=-1;
		            }
		            else if(cont==2)
		            {
		            	int ok=1;
		            	string codigo="",pnome="",unome="",sexo="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do codigo do funcionario: ";
		            		cin>>codigo;
		            		if(is_numeric(codigo.c_str())!=1 || codigo.size()>9)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	cout<<"Insira o valor do primeiro nome: ";
		            	cin>>pnome;
		            	cout<<"Insira o valor do ultimo nome: ";
		            	cin>>unome;
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o sexo (M/F): ";
		            		cin>>sexo;
		            		if(sexo=="M" || sexo=="m" || sexo=="F" || sexo=="f")
		            		{
		            			ok=1;
		            			toUpString(sexo);
		            		}
		            		else
		            		{
		            			ok=0;
		            			cout<<"Sexo invalido, tente novamente"<<endl;
		            		}
		            	}while(ok!=1);
		            	query="insert into funcionario(codigo,pnome,unome,sexo) "
		            			"values ('"+codigo+"','"+pnome+"','"+unome+"','"+sexo+"');";
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	codigo="",pnome="",unome="",sexo="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            }
		            else if(cont==3)
		            {
		            	int ok=1;
		            	string numero="",nome="",tipo="",gravar="";
		            	string tamanho="",largura="",preco="",query="";
		            	do
		            	{
		            		cout<<"Insira o numero da peca(8 caracteres): ";
		            		cin>>numero;
		            		if(numero.size()!=8)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Toda peca possui nome iniciando com: Anel ou Cordao ou Placa!"<<endl;
		            		cout<<"Insira o nome da peca: ";
		            		fflush(stdin);
		            		getline(cin,nome);
		            		size_t anel = nome.find("Anel");
		            		size_t cordao = nome.find("Cordao");
		            		size_t placa = nome.find("Placa");
		            		if(anel==0 || cordao==0 || placa==0)
		            			ok=1;
		            		else
		            		{
		            			ok=0;
		            			cout<<"Nome de peca invalido!"<<endl;
		            			continue;
		            		}
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o tamanho da peca(cm): ";
		            		cin>>tamanho;
		            		if(is_numeric(tamanho.c_str())!=1 || tamanho.size()>10)
		            		{
		            			ok=0;
		            			cout<<"Tamanho invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira a largura da peca(cm): ";
		            		cin>>largura;
		            		if(is_numeric(largura.c_str())!=1 || largura.size()>10)
		            		{
		            			ok=0;
		            			cout<<"Largura invalida"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Tipos de pecas possiveis sao: ouro ou prata ou folheado!"<<endl;
		            		cout<<"Insira o tipo da peca: ";
		            		fflush(stdin);
		            		getline(cin,tipo);
		            		size_t ouro = tipo.find("ouro");
		            		size_t prata = tipo.find("prata");
		            		size_t folheado = tipo.find("folheado");
		            		if(ouro==0 || prata==0 || folheado==0)
		            			ok=1;
		            		else
		            		{
		            			ok=0;
		            			cout<<"Nome de peca invalido!"<<endl;
		            			continue;
		            		}
		            	}while(ok!=1);
	            		size_t placa = nome.find("Placa");
	            		if(placa==0)
	            		{
	            			do
	            			{
	            				ok=1;
	            				cout<<"Insira a frase para se gravar na placa: "<<endl;
	            				fflush(stdin);
	            				getline(cin,gravar);
	            				if(gravar.size()>30)
	            				{
	            					ok=0;
	            					cout<<"Nome de placa muito grande!"<<endl;
	            					continue;
	            				}
	            				else
	            					ok=1;
	            			}while(ok!=1);
	            		}
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o preco: ";
		            		cin>>preco;
		            		if(is_numeric(preco.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Preco invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	if(placa==0)
		            	{
		            		query="insert into peca(numero,nome,tamanho,largura,tipo,preco,gravar) "
		            				"values ('"+numero+"','"+nome+"','"+tamanho+"','"+largura+"','"+
		            				tipo+"','"+preco+"','"+gravar+"');";
		            	}
		            	else
		            	{
		            		query="insert into peca(numero,nome,tamanho,largura,tipo,preco) "
		            				"values ('"+numero+"','"+nome+"','"+tamanho+"','"+largura+"','"+
									tipo+"','"+preco+"');";
		            	}
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	ok=1;
		            	numero="",nome="",tipo="",gravar="";
		            	tamanho="",largura="",preco="",query="";
		            }
		            else if(cont==4)
		            {
		            	int ok=1;
		            	string forma="",cpf_cliente="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf (11 digitos): ";
		            		cin>>cpf_cliente;
		            		if(valida_cpf(cpf_cliente)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
							ok=1;
							cout<<"Formas de Pagamento: boleto ou cartao ou carne!"<<endl;
							cout<<"Insira a forma de pagamento: ";
							fflush(stdin);
							getline(cin,forma);
							size_t boleto = forma.find("boleto");
							size_t cartao = forma.find("cartao");
							size_t carne = forma.find("carne");
							if(boleto==0 || cartao==0 || carne==0)
								ok=1;
							else
							{
								ok=0;
								cout<<"Forma de pagamento invalida!"<<endl;
								continue;
							}
		            	}while(ok!=1);
		            	query="insert into pagamento(forma,cpf_cliente) "
		            			"values ('"+forma+"','"+cpf_cliente+"');";
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	cpf_cliente="",forma=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	ok=1;
		            }
		            else if(cont==5)
		            {
		            	int ok=1;
		            	string codigo="",nome="",parentesco="",sexo="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do codigo do funcionario relacionado: ";
		            		cin>>codigo;
		            		if(is_numeric(codigo.c_str())!=1 || codigo.size()>9)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	cout<<"Insira o nome do dependente: ";
		            	cin>>nome;
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o sexo (M/F): ";
		            		cin>>sexo;
		            		if(sexo=="M" || sexo=="m" || sexo=="F" || sexo=="f")
		            		{
		            			ok=1;
		            			toUpString(sexo);
		            		}
		            		else
		            		{
		            			ok=0;
		            			cout<<"Sexo invalido, tente novamente"<<endl;
		            		}
		            	}while(ok!=1);
		            	cout<<"Insira o parentesco com o funcionario: ";
		            	cin>>parentesco;
		            	query="insert into familiar(codigo_funcionario,nome,parentesco,sexo) "
		            			"values ('"+codigo+"','"+nome+"','"+parentesco+"','"+sexo+"');";
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	codigo="",nome="",parentesco="",sexo="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	ok=1;
		            }
		            else if(cont==6)
		            {
		            	int ok=1;
		            	string cpf="",codigo_barras="",parcelas="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf (11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o codigo de barras(20 digitos): ";
		            		cin>>codigo_barras;
		            		if(codigo_barras.size()!=20 || is_numeric(codigo_barras.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Codigo de barras invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o numero de parcelas: ";
		            		cin>>parcelas;
		            		if(is_numeric(parcelas.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero de parcelas invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	query="insert into boleto(forma_pagamento,cpf_cliente,codigo_barras,parcelas) "
		            			"values ('boleto','"+cpf+"','"+codigo_barras+"','"+parcelas+"');";
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	cpf="",codigo_barras="",parcelas="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	ok=1;
		            }
		            else if(cont==7)
		            {
		            	int dia=0,mes=0,ano=0,ok=1;
		            	string cpf="",vencimento="",numero="",parcelas="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf (11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o numero do cartao(16 digitos): ";
		            		cin>>numero;
		            		if(numero.size()!=16 || is_numeric(numero.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero do cartao invalido!"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira a data de vencimento (dd-mm-aaaa): ";
		            		scanf("%d-%d-%d",&dia,&mes,&ano);
		            		fflush(stdin);
		            		if(valida_data(dia,mes,ano)!=1)
		            		{
		            			ok=0;
		            			cout<<"Data invalida"<<endl;
		            		}
		            		else
		            		{

		            			vencimento = SSTR(dia)+"-"+SSTR(mes)+"-"+SSTR(ano);
		            			if(ok==1)
		            				ok=1;
		            		}
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o numero de parcelas: ";
		            		cin>>parcelas;
		            		if(is_numeric(parcelas.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero de parcelas invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	query="insert into cartao(forma_pagamento,cpf_cliente,numero,vencimento,parcelas) "
		            			"values ('cartao','"+cpf+"','"+numero+"','"+vencimento+"','"+parcelas+"');";
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	cpf="",numero="",vencimento="",parcelas="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	ok=1;
		            }
		            else if(cont==8)
		            {
		            	int ok=1;
		            	string cpf="",desconto="",parcelas="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf (11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o desconto: ";
		            		cin>>desconto;
		            		if(is_numeric(desconto.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Codigo de barras invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o numero de parcelas: ";
		            		cin>>parcelas;
		            		if(is_numeric(parcelas.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero de parcelas invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	query="insert into carne(forma_pagamento,cpf_cliente,desconto,parcelas) "
		            			"values ('boleto','"+cpf+"','"+desconto+"','"+parcelas+"');";
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	cpf="",desconto="",parcelas="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	ok=1;
		            }
		            else if(cont==9)
		            {
		            	int ok=1;
		            	string numero="", cpf="",codigo="", query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf (11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o numero da peca(8 caracteres): ";
		            		cin>>numero;
		            		if(numero.size()!=8)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o codigo do funcionario relacionado: ";
		            		cin>>codigo;
		            		if(is_numeric(codigo.c_str())!=1 || codigo.size()>9)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	query="insert into registra_pedido(cpf_cliente,num_peca,codigo_funcionario) "
		            			"values ('"+cpf+"','"+numero+"','"+codigo+"');";
		            	cout<<"Detalhes da Insercao:\n"<<query<<endl;
		            	inserir(conn,query.c_str());
		            	cpf="",numero="",codigo="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	ok=1;
		            }
		            else if(cont==10)
		            {
		            	menu(conn);
		            }
		            else if(cont==11)
		            {
		            	PQfinish(conn);
		            	sair();
		            }
		        }
		    }
}

int inserir(PGconn *conn, const char *con)
{
	PGresult *result; // PGresult armazenará o resultado de uma consulta
	char * insercao = new char [500];
	strcpy(insercao,con);
	result = PQexec(conn, insercao);
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		cout<<"\nOcorreu uma erro na sua insercao, detalhes abaixo: \n";
		cout << PQerrorMessage(conn);
		PQclear(result); // libera os dados armazenados
		PQreset(conn); // reseta a conexao
		system("pause");
		return 0;
	}
	else
		cout<<"\nSua insercao foi realizada com sucesso!"<<endl;
	// Fecha o acesso aos registros
	PQclear(result);
	system("pause");
	return 1;
}
int menu_apagar(PGconn *conn)
{
	int key=0,cont=1;
	    while(1)
	    {
	        system("cls");
	        printf("%c=============================Apagar Tabela===================================%c\n",201,187);
	        printf("%c %s Uma Tabela inteira                                                        %c\n", 186,(cont == 1)? "X": " ",186);
	        printf("%c %s Uma(s) tupla(s) de Cliente por nome                                       %c\n",186,(cont == 2) ? "X": " ",186);
	        printf("%c %s Uma(s) tupla(s) de Funcionario por nome                                   %c\n",186, (cont == 3)? "X": " ",186);
	        printf("%c %s Uma tupla de Peca por numero                                              %c\n",186, (cont == 4)? "X": " ",186);
	        printf("%c %s Uma tupla de Pagamento por cpf                                            %c\n",186, (cont == 5)? "X": " ",186);
	        printf("%c %s Uma(s) tupla(s) de Familiar por nome                                      %c\n",186, (cont == 6)? "X": " ",186);
	        printf("%c %s Uma tupla de Boleto por codigo de barras                                  %c\n",186, (cont == 7)? "X": " ",186);
	        printf("%c %s Uma tupla de Cartao por numero                                            %c\n",186, (cont == 8)? "X": " ",186);
	        printf("%c %s Uma tupla de Carne  por cpf                                               %c\n",186, (cont == 9)? "X": " ",186);
	        printf("%c %s Uma tupla de Registra_pedido por cpf do cliente e numero da peca          %c\n",186, (cont == 10)? "X": " ",186);
	        printf("%c %s Voltar ao Menu Principal                                                  %c\n",186, (cont == 11)? "X": " ",186);
	        printf("%c %s Sair                                                                      %c\n",186,(cont == 12) ? "X": " ",186);
	        printf("%c                                                                             %c\n",186,186);
	        printf("%c=============================================================================%c\n",200,188);
	        key = getch();
	        if (key == KEY_ESC)
	            return 0;
	        else if(key == KEY_UP)
	        {
	            if(cont==0)
	                cont++;
	            else if(cont>=1 && cont<=12)
	                cont--;
	        }
	        else if(key == KEY_DOWN)
	        {
	            if(cont>=0 && cont<=11)
	                cont++;
	            else if(cont==11)
	                cont--;
	        }
	        else if(key==KEY_ENTER)
	        {
	            if(cont==1)
	            {
	            	int numero=-1;
	            	do
	            	{
	            		system("cls");
	            		cout<<"Digite uma opcao: \n";
						cout<<"1 - Cliente"<<endl;
						cout<<"2 - Funcionario"<<endl;
						cout<<"3 - Peca"<<endl;
						cout<<"4 - Pagamento"<<endl;
						cout<<"5 - Familiar"<<endl;
						cout<<"6 - Boleto"<<endl;
						cout<<"7 - Cartao"<<endl;
						cout<<"8 - Carne"<<endl;
						cout<<"9 - Registra_pedido"<<endl;
						cout<<"10 - Voltar ao menu anterior"<<endl;
						cin>>numero;
						if(numero>=1 && numero<=9)
							apaga_Tabelas(numero,conn);
						else if(numero==10)
							menu_apagar(conn);
	            	}
					while(numero<1 || numero>10);
	            }
	            else if(cont==2)
	            {
	            	string nome="",query="";
	            	cout<<"Insira o nome do cliente: ";
	            	cin>>nome;
	            	query = "delete"
	            			" from cliente"
	            			" where pnome='"+nome+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==3)
	            {
	            	string nome="",query="";
	            	cout<<"Insira o nome do funcionario: ";
	            	cin>>nome;
	            	query = "delete"
	            	" from funcionario"
	           	    " where pnome='"+nome+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==4)
	            {
	            	string peca="",query="";
	            	cout<<"Insira o numero da peca: ";
	            	cin>>peca;
	            	query = "delete"
	            			" from peca"
	            			" where numero='"+peca+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==5)
	            {
	            	string cpf_pagamento="",query="";
	            	cout<<"Insira o cpf do cliente: ";
	            	cin>>cpf_pagamento;
	            	query = "delete"
	            			" from pagamento"
	            			" where cpf_cliente='"+cpf_pagamento+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==6)
	            {
	            	string nome_familiar="",query="";
	            	cout<<"Insira o nome do familiar: ";
	            	cin>>nome_familiar;
	            	query = "delete"
	            			" from familiar"
	            			" where nome='"+nome_familiar+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==7)
	            {
	            	string codigo_boleto="",query="";
	            	cout<<"Insira o codigo do boleto: ";
	            	cin>>codigo_boleto;
	            	query = "delete"
	            			" from boleto"
	            			" where codigo_barras='"+codigo_boleto+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==8)
	            {
	            	string numero_cartao="",query="";
	            	cout<<"Insira o numero do cartao: ";
	            	cin>>numero_cartao;
	            	query = "delete"
	            			" from cartao"
	            			" where numero='"+numero_cartao+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==9)
	            {
	            	string cpf_carne="",query="";
	            	cout<<"Insira o cpf do cliente: ";
	            	cin>>cpf_carne;
	            	query = "delete"
	            			" from carne"
	            			" where cpf_cliente='"+cpf_carne+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==10)
	            {
	            	string cpf_cliente="",num_peca="",query="";
	            	cout<<"Insira o cpf do cliente: ";
	            	cin>>cpf_cliente;
	            	cout<<"Insira o numero da peca: ";
	            	cin>>num_peca;
	            	query = "delete"
	            			" from registra_pedido"
	            			" where cpf_cliente='"+cpf_cliente+"' and nuM_peca='"+num_peca+"';";
	            	apaga(conn,query.c_str());
	            }
	            else if(cont==11)
	            {
	            	menu(conn);

	            }
	            else if(cont==12)
	            {
	            	//Fecha a conexão com o banco
	            	PQfinish(conn);
	            	sair();
	            }

	        }
	    }
}
void apaga_Tabelas(int numero, PGconn* conn)
{

	char * query = new char [500];
	string resposta="";
	cout<<"Voce esta a deletar uma tabela inteira, continuar (s/n)?: ";
	cin>>resposta;
	if(resposta=="n" || resposta=="N")
		menu_apagar(conn);
	else
	{
		switch(numero)
		{
			case 1:
				strcpy(query,"delete from cliente");
				apaga(conn, query);
				break;
			case 2:
				strcpy(query,"delete from funcionario");
				apaga(conn, query);
				break;
			case 3:
				strcpy(query,"delete from peca");
				apaga(conn, query);
				break;
			case 4:
				strcpy(query,"delete from pagamento");
				apaga(conn, query);
				break;
			case 5:
				strcpy(query,"delete from familiar");
				apaga(conn, query);
				break;
			case 6:
				strcpy(query,"delete from boleto");
				apaga(conn, query);
				break;
			case 7:
				strcpy(query,"delete from cartao");
				apaga(conn, query);
				break;
			case 8:
				strcpy(query,"delete from carne");
				apaga(conn, query);
				break;
			case 9:
				strcpy(query,"delete from registra_pedido");
				apaga(conn, query);
				break;
		}
	}
}
int apaga(PGconn *conn, const char *con)
{
	PGresult *result; // PGresult armazenará o resultado de uma consulta
	char * apaga = new char [500];
	string res;
	strcpy(apaga,con);
	result = PQexec(conn, apaga);
	res = PQcmdTuples(result);
	if ( PQresultStatus(result) == PGRES_COMMAND_OK && res!="0")
		cout<<"\nSua delecao foi realizada com sucesso!"<<endl;
	else
	{
		cout<<"\nOcorreu uma erro na sua delecao, detalhes abaixo: \n";
		cout << PQerrorMessage(conn);
		if(res=="0")
			cout<<"Valor pesquisado nao exite na tabela!"<<endl;
		PQclear(result); // libera os dados armazenados
		PQreset(conn); // reseta a conexao
		system("pause");
		return 0;
	}
	PQclear(result);
	system("pause");
	return 1;
}

int menu_atualizar(PGconn *conn)
{
	int key=0,cont=1;
		    while(1)
		    {
		        system("cls");
		        printf("%c===============================Atualizar Tabela==============================%c\n",201,187);
		        printf("%c %s Cliente                                                                   %c\n", 186,(cont == 1)? "X": " ",186);
		        printf("%c %s Funcionario                                                               %c\n",186,(cont == 2) ? "X": " ",186);
		        printf("%c %s Peca                                                                      %c\n",186, (cont == 3)? "X": " ",186);
		        printf("%c %s Pagamento                                                                 %c\n",186, (cont == 4)? "X": " ",186);
		        printf("%c %s Familiar                                                                  %c\n",186, (cont == 5)? "X": " ",186);
		        printf("%c %s Boleto                                                                    %c\n",186, (cont == 6)? "X": " ",186);
		        printf("%c %s Cartao                                                                    %c\n",186, (cont == 7)? "X": " ",186);
		        printf("%c %s Carne                                                                     %c\n",186, (cont == 8)? "X": " ",186);
		        printf("%c %s Registra_Pedido                                                           %c\n",186, (cont == 9)? "X": " ",186);
		        printf("%c %s Voltar ao Menu Principal                                                  %c\n",186, (cont == 10)? "X": " ",186);
		        printf("%c %s Sair                                                                      %c\n",186,(cont == 11) ? "X": " ",186);
		        printf("%c                                                                             %c\n",186,186);
		        printf("%c=============================================================================%c\n",200,188);
		        key = getch();
		        if (key == KEY_ESC)
		            return 0;
		        else if(key == KEY_UP)
		        {
		            if(cont==0)
		                cont++;
		            else if(cont>=1 && cont<=11)
		                cont--;
		        }
		        else if(key == KEY_DOWN)
		        {
		            if(cont>=0 && cont<=10)
		                cont++;
		            else if(cont==11)
		                cont--;
		        }
		        else if(key==KEY_ENTER)
		        {
		            if(cont==1)
		            {
		            	int dia,mes,ano,ok=1;
		            	string cpf="",cpfOld="",pnome="",unome="",data="",sexo="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf (11 digitos) que deseja alterar: ";
		            		cin>>cpfOld;
		            		if(valida_cpf(cpfOld)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo cpf (11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	cout<<"Insira o novo valor do primeiro nome: ";
		            	cin>>pnome;
		            	cout<<"Insira o novo valor do ultimo nome: ";
		            	cin>>unome;
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo sexo (M/F): ";
		            		cin>>sexo;
		            		if(sexo=="M" || sexo=="m" || sexo=="F" || sexo=="f")
		            		{
		            			ok=1;
		            			toUpString(sexo);
		            		}
		            		else
		            		{
		            			ok=0;
		            			cout<<"Sexo invalido, tente novamente"<<endl;
		            		}
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira a nova data de nascimento (dd-mm-aaaa): ";
		            		scanf("%d-%d-%d",&dia,&mes,&ano);
		            		fflush(stdin);
		            		if(valida_data(dia,mes,ano)!=1)
		            		{
		            			ok=0;
		            			cout<<"Data invalida"<<endl;
		            		}
		            		else
		            		{

		            			data = SSTR(dia)+"-"+SSTR(mes)+"-"+SSTR(ano);
		            			if(ok==1)
		            				ok=1;
		            		}
		            	}while(ok!=1);
		            	query="update cliente"
		            		" set cpf='"+cpf+"', pnome='"+pnome+"', unome='"+unome+"', data_nascimento='"+data+"', sexo='"+sexo+"'"
		            		" where cpf='"+cpfOld+"';";
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	cpfOld="",cpf="",pnome="",unome="",data="",sexo="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	dia=mes=ano=-1;
		            }
		            else if(cont==2)
		            {
		            	int ok=1;
		            	string codigo="",codigoOld="",pnome="",unome="",sexo="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do codigo do funcionario que deseja alterar: ";
		            		cin>>codigoOld;
		            		if(is_numeric(codigoOld.c_str())!=1 || codigoOld.size()>9)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo valor do codigo do funcionario: ";
		            		cin>>codigo;
		            		if(is_numeric(codigo.c_str())!=1 || codigo.size()>9)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	cout<<"Insira o novo valor do primeiro nome: ";
		            	cin>>pnome;
		            	cout<<"Insira o novo valor do ultimo nome: ";
		            	cin>>unome;
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo sexo (M/F): ";
		            		cin>>sexo;
		            		if(sexo=="M" || sexo=="m" || sexo=="F" || sexo=="f")
		            		{
		            			ok=1;
		            			toUpString(sexo);
		            		}
		            		else
		            		{
		            			ok=0;
		            			cout<<"Sexo invalido, tente novamente"<<endl;
		            		}
		            	}while(ok!=1);
		            	query="update funcionario"
		            			" set codigo='"+codigo+"', pnome='"+pnome+"', unome='"+unome+"', sexo='"+sexo+"'"
								" where codigo='"+codigoOld+"';";
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	codigoOld="",pnome="",unome="",sexo="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	ok=1;
		            }
		            else if(cont==3)
		            {
		            	int ok=1;
		            	string numero="",numeroOld="",nome="",tipo="",gravar="";
		            	string tamanho="",largura="",preco="",query="";
		            	do
		            	{
		            		cout<<"Insira o numero da peca que deseja alterar(8 caracteres): ";
		            		cin>>numeroOld;
		            		if(numeroOld.size()!=8)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o numero da nova peca(8 caracteres): ";
		            		cin>>numero;
		            		if(numero.size()!=8)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Toda peca possui nome iniciando com: Anel ou Cordao ou Placa!"<<endl;
		            		cout<<"Insira o nome da nova peca: ";
		            		fflush(stdin);
		            		getline(cin,nome);
		            		size_t anel = nome.find("Anel");
		            		size_t cordao = nome.find("Cordao");
		            		size_t placa = nome.find("Placa");
		            		if(anel==0 || cordao==0 || placa==0)
		            			ok=1;
		            		else
		            		{
		            			ok=0;
		            			cout<<"Nome de peca invalido!"<<endl;
		            			continue;
		            		}
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo tamanho da peca(cm): ";
		            		cin>>tamanho;
		            		if(is_numeric(tamanho.c_str())!=1 || tamanho.size()>10)
		            		{
		            			ok=0;
		            			cout<<"Tamanho invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira a nova largura da peca(cm): ";
		            		cin>>largura;
		            		if(is_numeric(largura.c_str())!=1 || largura.size()>10)
		            		{
		            			ok=0;
		            			cout<<"Largura invalida"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Tipos de pecas possiveis sao: ouro ou prata ou folheado!"<<endl;
		            		cout<<"Insira o novo tipo da peca: ";
		            		fflush(stdin);
		            		getline(cin,tipo);
		            		size_t ouro = tipo.find("ouro");
		            		size_t prata = tipo.find("prata");
		            		size_t folheado = tipo.find("folheado");
		            		if(ouro==0 || prata==0 || folheado==0)
		            			ok=1;
		            		else
		            		{
		            			ok=0;
		            			cout<<"Nome de peca invalido!"<<endl;
		            			continue;
		            		}
		            	}while(ok!=1);
	            		size_t placa = nome.find("Placa");
	            		if(placa==0)
	            		{
	            			do
	            			{
	            				ok=1;
	            				cout<<"Insira a nova frase para se gravar na placa: "<<endl;
	            				fflush(stdin);
	            				getline(cin,gravar);
	            				if(gravar.size()>30)
	            				{
	            					ok=0;
	            					cout<<"Nome de placa muito grande!"<<endl;
	            					continue;
	            				}
	            				else
	            					ok=1;
	            			}while(ok!=1);
	            		}
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo preco: ";
		            		cin>>preco;
		            		if(is_numeric(preco.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Preco invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	if(placa==0)
		            	{
		            		query="update peca"
		            				" set numero='"+numero+"', nome='"+nome+"', tamanho='"+tamanho+"', largura='"+largura+"', tipo='"+tipo+"', gravar='"+gravar+"'"
									" where numero='"+numeroOld+"';";
		            	}
		            	else
		            	{
		            		query="update peca"
		            				" set numero='"+numero+"', nome='"+nome+"', tamanho='"+tamanho+"', largura='"+largura+"', tipo='"+tipo+"'"
									" where numero='"+numeroOld+"';";
		            	}
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	numero="",numeroOld="",nome="",tamanho="",largura="",tipo="",gravar="",query=""; //é necessario resetar as variaveis para nao conter lixo de uma insercao anterior
		            	ok=1;
		            }
		            else if(cont==4)
		            {
		            	int ok=1;
		            	string forma="",cpf_cliente="",cpf_clienteOld="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf que deseja alterar(11 digitos): ";
		            		cin>>cpf_clienteOld;
		            		if(valida_cpf(cpf_clienteOld)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo valor do cpf(11 digitos): ";
		            		cin>>cpf_cliente;
		            		if(valida_cpf(cpf_cliente)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
							ok=1;
							cout<<"Formas de Pagamento: boleto ou cartao ou carne!"<<endl;
							cout<<"Insira a nova forma de pagamento: ";
							fflush(stdin);
							getline(cin,forma);
							size_t boleto = forma.find("boleto");
							size_t cartao = forma.find("cartao");
							size_t carne = forma.find("carne");
							if(boleto==0 || cartao==0 || carne==0)
								ok=1;
							else
							{
								ok=0;
								cout<<"Forma de pagamento invalida!"<<endl;
								continue;
							}
		            	}while(ok!=1);
		            	query="update pagamento"
		            			" set forma='"+forma+"', cpf_cliente='"+cpf_cliente+"'"
								" where cpf_cliente='"+cpf_clienteOld+"';";
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	cpf_cliente=cpf_clienteOld=forma="";
		            	ok=1;
		            }
		            else if(cont==5)
		            {
		            	int ok=1;
		            	string codigo="",codigoOld="",nome="",parentesco="",sexo="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do codigo do funcionario do familiar que se deseja alterar: ";
		            		cin>>codigo;
		            		if(is_numeric(codigoOld.c_str())!=1 || codigoOld.size()>9)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo valor do codigo do funcionario do familiar: ";
		            		cin>>codigo;
		            		if(is_numeric(codigo.c_str())!=1 || codigo.size()>9)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	cout<<"Insira o novo nome do dependente: ";
		            	cin>>nome;
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo sexo (M/F): ";
		            		cin>>sexo;
		            		if(sexo=="M" || sexo=="m" || sexo=="F" || sexo=="f")
		            		{
		            			ok=1;
		            			toUpString(sexo);
		            		}
		            		else
		            		{
		            			ok=0;
		            			cout<<"Sexo invalido, tente novamente"<<endl;
		            		}
		            	}while(ok!=1);
		            	cout<<"Insira o novo parentesco com o funcionario: ";
		            	cin>>parentesco;
		            	query="update familiar"
		            			" set nome='"+nome+"', sexo='"+sexo+"', parentesco='"+parentesco+"'"
								" where codigo_funcionario='"+codigoOld+"';";
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	nome=sexo=parentesco=codigoOld=codigo="";
		            	ok=1;
		            }
		            else if(cont==6)
		            {
		            	int ok=1;
		            	string cpf="",codigo_barras="",codigo_barrasOld="",parcelas="",query="";
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o codigo de barras que se deseja alterar(20 digitos): ";
		            		cin>>codigo_barrasOld;
		            		if(codigo_barrasOld.size()!=20 || is_numeric(codigo_barrasOld.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Codigo de barras invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo valor do codigo de barras(20 digitos): ";
		            		cin>>codigo_barras;
		            		if(codigo_barras.size()!=20 || is_numeric(codigo_barras.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Codigo de barras invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo valor do cpf do cliente que fez o pagamento(11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo numero de parcelas do cliente: ";
		            		cin>>parcelas;
		            		if(is_numeric(parcelas.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero de parcelas invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	query=	"update boleto"
		            			" set forma_pagamento='boleto', cpf_cliente='"+cpf+"', codigo_barras='"+codigo_barras+"', parcelas='"+parcelas+"'"
								" where codigo_barras='"+codigo_barrasOld+"';";
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	cpf=codigo_barras=codigo_barrasOld=parcelas="";
		            	ok=1;
		            }
		            else if(cont==7)
		            {
		            	int dia=0,mes=0,ano=0,ok=1;
		            	string cpf="",vencimento="",numero="",numeroOld="",parcelas="",query="";
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o numero do cartao que se deseja atualizar(16 digitos): ";
		            		cin>>numeroOld;
		            		if(numeroOld.size()!=16 || is_numeric(numeroOld.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero do cartao invalido!"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo numero do cartao(16 digitos): ";
		            		cin>>numero;
		            		if(numero.size()!=16 || is_numeric(numero.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero do cartao invalido!"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo valor do cpf (11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira a nova data de vencimento (dd-mm-aaaa): ";
		            		scanf("%d-%d-%d",&dia,&mes,&ano);
		            		fflush(stdin);
		            		if(valida_data(dia,mes,ano)!=1)
		            		{
		            			ok=0;
		            			cout<<"Data invalida"<<endl;
		            		}
		            		else
		            		{

		            			vencimento = SSTR(dia)+"-"+SSTR(mes)+"-"+SSTR(ano);
		            			if(ok==1)
		            				ok=1;
		            		}
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo numero de parcelas: ";
		            		cin>>parcelas;
		            		if(is_numeric(parcelas.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero de parcelas invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	query=	"update cartao"
		            			" set forma_pagamento='cartao', cpf_cliente='"+cpf+"', vencimento='"+vencimento+"', parcelas='"+parcelas+"', numero='"+numero+"'"
								" where numero='"+numeroOld+"';";
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	cpf=vencimento=numero=numeroOld=parcelas="";
		            	ok=1;
		            }
		            else if(cont==8)
		            {
		            	int ok=1;
		            	string cpf="",cpfOld="",desconto="",parcelas="",query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf que deseja atualizar(11 digitos): ";
		            		cin>>cpfOld;
		            		if(valida_cpf(cpfOld)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o valor do novo cpf(11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo valor de desconto: ";
		            		cin>>desconto;
		            		if(is_numeric(desconto.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Codigo de barras invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		ok=1;
		            		cout<<"Insira o novo numero de parcelas: ";
		            		cin>>parcelas;
		            		if(is_numeric(parcelas.c_str())!=1)
		            		{
		            			ok=0;
		            			cout<<"Numero de parcelas invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	query=	"update carne"
		            			" set forma_pagamento='carne', cpf_cliente='"+cpf+"', desconto='"+desconto+"', parcelas='"+parcelas+"'"
								" where cpf_cliente='"+cpfOld+"';";
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	cpf=cpfOld=desconto=parcelas="";
		            	ok=1;
		            }
		            else if(cont==9)
		            {
		            	int ok=1;
		            	string numero="",numeroOld="",cpf="",cpfOld="",codigo="", query="";
		            	do
		            	{
		            		cout<<"Insira o valor do cpf do cliente que se deseja alterar (11 digitos): ";
		            		cin>>cpfOld;
		            		if(valida_cpf(cpfOld)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o numero da peca que o cliente pediu que se deseja alterar(8 caracteres): ";
		            		cin>>numeroOld;
		            		if(numeroOld.size()!=8)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo valor do cpf do cliente(11 digitos): ";
		            		cin>>cpf;
		            		if(valida_cpf(cpf)!=1)
		            		{
		            			ok=0;
		            			cout<<"CPF invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo numero da peca que o cliente pediu(8 caracteres): ";
		            		cin>>numero;
		            		if(numero.size()!=8)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	do
		            	{
		            		cout<<"Insira o novo codigo do funcionario relacionado: ";
		            		cin>>codigo;
		            		if(is_numeric(codigo.c_str())!=1 || codigo.size()>9)
		            		{
		            			ok=0;
		            			cout<<"Codigo invalido"<<endl;
		            			continue;
		            		}
		            		else
		            			ok=1;
		            	}while(ok!=1);
		            	query=	"update registra_pagamento"
		            			" set cpf_cliente='"+cpf+"', num_peca='"+numero+"', codigo_funcionario='"+codigo+"'"
								" where cpf_cliente='"+cpfOld+"', num_peca='"+numeroOld+"';";
		            	cout<<"Detalhes da Atualizacao:\n"<<query<<endl;
		            	atualiza(conn,query.c_str());
		            	cpf=cpfOld=numero=numeroOld=codigo="";
		            	ok=1;
		            }
		            else if(cont==10)
		            {
		            	menu(conn);
		            }
		            else if(cont==11)
		            {
		            	PQfinish(conn);
		            	sair();
		            }
		        }
		    }
}

int atualiza(PGconn *conn, const char *con)
{
	PGresult *result; // PGresult armazenará o resultado de uma consulta
	char * insercao = new char [500];
	strcpy(insercao,con);
	result = PQexec(conn, insercao);
	string res = PQcmdTuples(result);
	if (PQresultStatus(result) == PGRES_COMMAND_OK && res!="0")
		cout<<"\nSua atualizacao foi realizada com sucesso!"<<endl;
	else
	{
		cout<<"\nOcorreu uma erro na sua atualizacao, detalhes abaixo: \n";
		cout << PQerrorMessage(conn);
		if(res=="0")
			cout<<"Nao existe ninguem cadastrado com o cpf informado!"<<endl;
		PQclear(result); // libera os dados armazenados
		PQreset(conn); // reseta a conexao
		system("pause");
		return 0;
	}
	// Fecha o acesso aos registros
	PQclear(result);
	system("pause");
	return 1;
}


















string getParamName(string parameter)
{
	return(parameter.substr(parameter.find("-")+1,parameter.find("=")-(parameter.find("-")+1)));
}
string getParamValue(string value)
{
	return(value.substr(value.find("=")+1,value.find(" ")-(value.find("=")+1)));
}
void helpOptions()
{
	cout << "\nOptions:" <<endl;
	cout << "\t[nome do host]: nome do host que queremos nos conectar." << endl;
	cout << "\t[numero da porta]:  porta de conexão com o PostgreSQL." << endl;
	cout << "\t[nome do BD]: nome do banco de dados que iremos nos conectar." << endl;
	cout << "\t[nome do usuario]: nome do usuario a realizar a conexao." << endl;
	cout << "\t[senha] = senha para o usuario a ser usado."<<endl<<endl;
	cout<<"Ex: Banco.exe host=localhost port=5432 dbname=joalheria user=jon password=159357"<<endl;
}
void pause (float tempo)
{
    if (tempo<0.001)
        return;
    float inst1=0, inst2=0;
    inst1 = (float)clock()/(float)CLOCKS_PER_SEC;
    while (inst2-inst1<tempo)
        inst2 = (float)clock()/(float)CLOCKS_PER_SEC;
    return;
}
void sair()
{
    printf("%15s Saindo...\n", " ");
    exit(0);
}
// função personalizada que permite verificar se uma
// string contém apenas dígitos de 0 a 9
int is_numeric(const char *str)
{
  int res = 1; // passou no teste
  // vamos percorrer todos os caracteres da string
  // e testar se cada um é um dígito. Se não
  // for nós marcamos res como 0 e retornamos
  while(*str)
  {
    if(!isdigit(*str)and (*str != ','))
    {
      res = 0;
      break;
    }
    // Passa para o próximo caractere
    str++;
  }
  return res;
}
int valida_cpf(string str)
{
	if(is_numeric(str.c_str())==1 and str.size()==11)
		return 1;
	else
		return 0;

}
int valida_data(int dia, int mes, int ano)
{
	int tag=1;
	if((dia>31)||(dia<=0))
		tag=0;
	if((mes>12)||(mes<=0))
		tag=0;
	if((ano>2014)||(ano<1900))
		tag=0;
	return tag;
}
void toUpString(string &str)
{
	for (int i=0; i<str.size(); i++)
		str[i]=toupper(str[i]);
}
