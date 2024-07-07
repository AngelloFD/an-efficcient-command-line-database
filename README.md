# C++ cmd application capable of handling 33+ millions of Peruvian citizens' data

## About

This is a C++ command line application that can handle 33+ millions of Peruvian citizens' data with the help of a Trie Tree. It can read, write, search, and delete the data. All data is stored in a binary file along with another for the position of each record in the data file assuring an efficient way of searching through all the records.

This project was developed as a final project for the Advanced Data Structures course at Universidad San Ignacio de Loyola (USIL) in Lima, Peru.
The requirements were as follows:

- The application must be able to handle 33+ millions of Peruvian citizens' data without the use of database management software.
- The application must save the data on the PC's disk.
- The application must be able to read, write, search, and delete the data.
  - The application must be able to search for a citizen by their ID if they exist, otherwise, it must say that said searched citizen doesn't exist.
  - The search must be done in memory.
- The application must be developed in C++.

## How does it work?

The application reads the data from a CSV file containing the citizens' data where the first column must always be the citizen's ID and the first row is the headers. The data is then stored in two binary files with the user's name of choice:

- The first file contains the data itself.
- The second file contains the position of each record in the data file.

The following is a flowchart of the application:

![tf-eda-maqueta drawio](https://github.com/AngelloFD/tf-eda/assets/75924779/4f6d40e1-1754-46aa-b43f-bfd78f0db4ff)

### Storing the data

First, the application creates the two binary files needed with the name that the user gave them. Then, reads the CSV file line by line, skipping the headers, and stores the data in the data file. The position of each record is stored in the positions file at the same time.

### Searching for a citizen

Since the search must be done in memory, the application will only read the positions file, a block of a defined number of records, and then search for the desired citizen. This way, the application can handle 33+ million records without a problem.

### Adding a new citizen

The application will ask the user for the new citizen's data and then append it to the data file and update the positions file. All new data will always be added at the end of the file.

### Deleting a citizen

Nowadays, it is not recommended to delete a user's data from a database. Instead, it is recommended to mark the record as deactivated because it can be used for monitoring or future metrics that one would like to do.
The application will ask the user for the ID of the citizen to delete. It will then search for the citizen and mark it as deactivated in the data file. The application will not remove the record from the file, but it will mark it as deactivated. This way, the application can handle the deletion of records without having to move the data.

### Metrics

This project can be run on Windows and Linux systems. The application was tested on a Windows 10 machine with an Intel Core i5-2320 CPU @ 3.00GHz 3.00 GHz and 8 GB of RAM and on a Linux machine running Lubuntu 22.04 with the same specs. The compiler used was g++ v13.2.0
In the best case (the record being searched for is the first one) it took 0.20 seconds to find and display the record while in the worst case (the record is not found) it took 45.38 seconds.

![imagen](https://github.com/AngelloFD/tf-eda/assets/75924779/3cff092a-f505-4869-986e-7441cc1087b7)
![imagen](https://github.com/AngelloFD/tf-eda/assets/75924779/521dbf9f-e8a9-4ac4-8969-e9b2076db501)

On the other hand, adding a record takes, in the best case (a record with the same ID is already found) and in the worst case (a record with the same ID is not found) the same amount of time as searching for a record but with the addition of 0.10 extra seconds.

![imagen](https://github.com/AngelloFD/tf-eda/assets/75924779/72e242cb-9339-4e82-8fb8-d3add8965ac9)

The same way with deactivating a record.

![imagen](https://github.com/AngelloFD/tf-eda/assets/75924779/56c2be58-b80f-413a-9e27-092b15ca7c1d)

With this, I've completed all the main objectives and requirements for the project's presentation, assuring an efficient way of storing and searching through 33+ million of data.
