//============================================================================
// Name        : dictionaryAttack_v0p1.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Salted and Unsalted Dictionary Attack Program
//============================================================================

#include <iostream>
#include <string>
#include <algorithm>
#include <set>
#include <chrono>
using namespace std;
using namespace chrono;

// ==========================================================================

#define PASSWORD_LENGTH 	4			// Definition of the password length
#define MAX_SALT			10			// Definition of the max value for the salt
bool foundPassword 			= false;	// Global variable to terminate the recursive function
string detectedPassword 	= ""; 		// Global variable to hold the detected password
int detectedSaltValue 		= 0;		// Global variable to hold the detected salt value

// alphabet array to be used in the generation of all possible combinations of length N
char alphabet[] = {		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
						'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
						'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
						'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
						'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
// ==========================================================================
// Getting the password from the user as an input to the program
string passwordInput()
{
	string inPass;
	cout << "Enter Password (" << PASSWORD_LENGTH <<" Characters Only): ";
	cin >> inPass;
	cout <<endl ;

	return inPass;
}

// ==========================================================================
// Getting the salt value selected by the user as an input to the program
int passwordSaltInput()
{
	int salt;
	cout << "Enter Salt number (non-negative integer - Max."<< MAX_SALT<< "): ";
	cin >> salt;
	cout << endl;

	return salt;
}


// ==========================================================================
// Calculating checksum as CRC function
unsigned int checksumCalc(const char *message)
{
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (message[i] != 0)
   {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--)
      {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

// ==========================================================================
// Getting all possible combinations of the a certain length using a provided alphabet
void printAllKLengthRec(char set[], string prefix, int n, int k, unsigned int passwordHash, bool salted)
{
	// terminate the recursion in case of finding the password
	if (foundPassword)
		return;

	// if we have a complete password, start the checking process
    if (k == 0)
    {
    	// handle the unsalted attack where we only check the hash of the password
    	if (!salted)
    	{
    		if (checksumCalc(prefix.c_str()) == passwordHash)
			{
				foundPassword = true;
				detectedPassword = prefix;
			}
    	}
    	// handle the salted attack where we try concatenated numbers to the password as salt
    	else
    	{
    		for(int i=0; i<=MAX_SALT; i++)
    		{
    			// concatenate the salt value with the string
    			string newPrefix = to_string(i) + prefix;
    			if (checksumCalc(newPrefix.c_str()) == passwordHash)
				{
					foundPassword = true;
					detectedPassword = prefix;
					detectedSaltValue = i;
					break;
				}
    		}
    	}
        return;
    }
    // Continue to work recursively until a complete word with the size N is generated
    for (int i = 0; i < n; i++)
    {
        string newPrefix;
        newPrefix = prefix + set[i];
        printAllKLengthRec(set, newPrefix, n, k - 1, passwordHash, salted);
    }
}

void printAllKLength(char set[], int k,int n, unsigned int passwordHash, bool salted)
{
	// driver function call to the recursive function
    printAllKLengthRec(set, "", n, k, passwordHash, salted);
}

// ==========================================================================
// unsalted attack code .. generate a dictionary and call the combinations generator
string unsaltedPasswordAttack(int passwordLength, unsigned int passwordHash)
{
	foundPassword = false;

	// salted flag is set to false to indicate that the attack is on unsalted password
	printAllKLength(alphabet, PASSWORD_LENGTH, 62, passwordHash, false);

	// return the detected password saved in global variable
	return detectedPassword;

}

// ==========================================================================
// salted attack code .. use the dictionary and call the combinations generator as well
string saltedPasswordAttack(int passwordLength, unsigned int passwordHash)
{
	// reset the found password flag to indicate a new fresh search for the password
	foundPassword = false;

	// salted flag is set to true to indicate that the attach is on salted password
	printAllKLength(alphabet, PASSWORD_LENGTH, 62, passwordHash, true);

	// return the detected password from the dictrionary attack
	return detectedPassword;
}

// ==========================================================================

int main() {

	string userPassword;
	string hashedPassword;
	int saltValue;

	// Step 0: get input password from the user
	userPassword = passwordInput();

	// ========================================================================================================
	// Part 1: Unsalted Password Dictionary Attack

	// calculate checksum of the password
	unsigned int passwordCheckSum = checksumCalc(userPassword.c_str());

	// start the timer to calculate the execution time
	auto unsaledStart = high_resolution_clock::now();

	// apply the dictionary attack with no salt added to the password
	string unsaltedPasswordDetected = unsaltedPasswordAttack(PASSWORD_LENGTH, passwordCheckSum);

	// stop the timer for execution time
	auto unsaledStop = high_resolution_clock::now();

	// print the detected password after the attack
	cout << "Detected Password is: " << unsaltedPasswordDetected << endl;

	// print the execution time
	auto unsaltedDuration = duration_cast<milliseconds>(unsaledStop - unsaledStart);
	cout << "Password detected in: " << unsaltedDuration.count() << " milliseconds." << endl << endl;


	// ========================================================================================================
	// Part 2: Salted Password Dictionary Attack

	// get the salt value from the user
	saltValue = passwordSaltInput();

	// concatenate the salt with the password
	string concatPassword = to_string(saltValue) + userPassword;

	// compute the checksum of the concatenated password
	unsigned int saltedPasswordCheckSum = checksumCalc(concatPassword.c_str());


	// start the time for calculation of execution time
	auto saledStart = high_resolution_clock::now();

	// call the attack function
	string saltedPasswordDetected = saltedPasswordAttack(PASSWORD_LENGTH, saltedPasswordCheckSum);

	// stop the timer for execution time
	auto saledStop = high_resolution_clock::now();

	// print the detected password and salt value
	cout << "Detected Password is:   " << saltedPasswordDetected << endl;
	cout << "Detected Salt Value is: " << detectedSaltValue << endl;


	// print the execution time
	auto saltedDuration = duration_cast<milliseconds>(saledStop - saledStart);
	cout << "Password detected in: " << saltedDuration.count() << " milliseconds." << endl << endl;

	return 0;
}
