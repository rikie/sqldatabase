This project aims to implement a tiny-SQL interpreter. The language will understand the grammar as per the project description. The interpreter is capable to take one line at a time or set of commands entered from a file. One a statement is read from the command line it performs the front end analysis such as lexical analysis, syntax and semantic analysis. After the statement entered is found conforming to the grammar of the language, a logical query plan is generated which is then optimized and then a physical query plan is generated which is then executed. To run the code, please see the first section on front end as well as the snapshots at the end of the report. The project report explains each stage of the tiny-SQL interpreter as follows:


a.	Front End-The use interface

b.	Lexical analyzer - Tokenizer

c.	Syntax-semantic analyzer-Parser

d.	Logical query plan generator

e.	Physical query plan generator

f.	Code generator

This project is not complete in the sense that the Storage manager is not uploaded as it was given by our professor. Nevertheless it is a complete design of a database system once storage manager(which manages memory etc.) with proper API is provided.