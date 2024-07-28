#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NIBBLE_SIZE 4
#define WORD_SIZE 8
#define KEY_SIZE 16

const char *SBOX[4][4] = {
    {"1001", "0100", "1010", "1011"},
    {"1101", "0001", "1000", "0101"},
    {"0110", "0010", "0000", "0011"},
    {"1100", "1110", "1111", "0111"}};

char key0[KEY_SIZE + 1] = {0}, key1[KEY_SIZE + 1] = {0}, key2[KEY_SIZE + 1] = {0};
int encryptionConstantMatrix[2][2] = {{1, 4}, {4, 1}};

int binaryToDecimal(const char *binary) {
    return (int)strtol(binary, NULL, 2);
}

void decimalToBinary(int decimal, int binaryStringSize, char *binaryStr) {
    for (int i = binaryStringSize - 1; i >= 0; --i) {
        binaryStr[i] = (decimal & 1) ? '1' : '0';
        decimal >>= 1;
    }
    binaryStr[binaryStringSize] = '\0';
}

void stringXOR(const char *a, const char *b, char *result) {
    for (int i = 0; i < strlen(a); i++) {
        result[i] = a[i] ^ b[i] ? '1' : '0';
    }
    result[strlen(a)] = '\0';
}

// Galois field Multiplication
int gfMul(int a, int b) {
    int product = 0; // the product of the multiplication

    while (b > 0) {
        if (b & 1) // if b is odd then add the first num i.e a into product result
            product ^= a;

        a <<= 1; // double first num

        // if a overflows beyond 4th bit
        if (a & (1 << 4))
            a ^= 0b10011; // XOR with irreducible polynomial with high term eliminated

        b >>= 1; // reduce second num
    }
    return product;
}

void nibbleSubstitution(const char *input, const char *SBOX[4][4], char *result) {
    for (int i = 0; i < strlen(input) / NIBBLE_SIZE; i++) {
        char str[NIBBLE_SIZE + 1] = {0};
        strncpy(str, input + (i * NIBBLE_SIZE), NIBBLE_SIZE);
        strcat(result, SBOX[binaryToDecimal(str) / 4][binaryToDecimal(str) % 4]);
    }
}

void shiftRow(const char *str, char *result) {
    // Swap 2nd and 4th nibble
    strncat(result, str, NIBBLE_SIZE);
    strncat(result, str + 3 * NIBBLE_SIZE, NIBBLE_SIZE);
    strncat(result, str + 2 * NIBBLE_SIZE, NIBBLE_SIZE);
    strncat(result, str + NIBBLE_SIZE, NIBBLE_SIZE);
}

void rotateNibble(const char *word, char *result) {
    strncpy(result, word + NIBBLE_SIZE, NIBBLE_SIZE);
    strncat(result, word, NIBBLE_SIZE);
}

void generateKeys(const char *key) {
    char w0[WORD_SIZE + 1] = {0}, w1[WORD_SIZE + 1] = {0};
    char w2[WORD_SIZE + 1] = {0}, w3[WORD_SIZE + 1] = {0};
    char w4[WORD_SIZE + 1] = {0}, w5[WORD_SIZE + 1] = {0};

    strncpy(w0, key, WORD_SIZE);
    strncpy(w1, key + WORD_SIZE, WORD_SIZE);

    char rotated[NIBBLE_SIZE * 2 + 1] = {0};
    rotateNibble(w1, rotated);

    char substituted[NIBBLE_SIZE * 2 + 1] = {0};
    nibbleSubstitution(rotated, SBOX, substituted);

    char xor_result[WORD_SIZE + 1] = {0};
    stringXOR(w0, "10000000", xor_result);
    stringXOR(xor_result, substituted, w2);

    stringXOR(w2, w1, w3);

    rotateNibble(w3, rotated);
    nibbleSubstitution(rotated, SBOX, substituted);

    stringXOR(w2, "00110000", xor_result);
    stringXOR(xor_result, substituted, w4);

    stringXOR(w4, w3, w5);

    strcat(key0, w0);
    strcat(key0, w1);

    strcat(key1, w2);
    strcat(key1, w3);

    strcat(key2, w4);
    strcat(key2, w5);
}

void getKeys() {
    printf("Key0: %s\n", key0);
    printf("Key1: %s\n", key1);
    printf("Key2: %s\n", key2);
}

void encrypt(const char *plainText, char *encryptedMsg) {
    // Round 0 - Add Key
    char roundZeroResult[KEY_SIZE + 1] = {0};
    stringXOR(plainText, key0, roundZeroResult);

    // Round 1 - Nibble Substitution -> Shift Row -> Mix Columns -> Add Key
    char shiftRowResult[KEY_SIZE + 1] = {0};
    char substituted[NIBBLE_SIZE * 4 + 1] = {0};
    nibbleSubstitution(roundZeroResult, SBOX, substituted);
    shiftRow(substituted, shiftRowResult);

    char matrix[2][2][NIBBLE_SIZE + 1] = {0};
    strncpy(matrix[0][0], shiftRowResult, NIBBLE_SIZE);
    strncpy(matrix[0][1], shiftRowResult + 2 * NIBBLE_SIZE, NIBBLE_SIZE);
    strncpy(matrix[1][0], shiftRowResult + NIBBLE_SIZE, NIBBLE_SIZE);
    strncpy(matrix[1][1], shiftRowResult + 3 * NIBBLE_SIZE, NIBBLE_SIZE);

    char sb[KEY_SIZE + 1] = {0};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            char tempResults[2][NIBBLE_SIZE + 1] = {0};
            for (int k = 0; k < 2; k++) {
                char binaryStr[NIBBLE_SIZE + 1] = {0};
                decimalToBinary(gfMul(encryptionConstantMatrix[i][k], binaryToDecimal(matrix[k][j])), NIBBLE_SIZE, binaryStr);
                strcpy(tempResults[k], binaryStr);
            }
            char tempResult[NIBBLE_SIZE + 1] = {0};
            stringXOR(tempResults[0], tempResults[1], tempResult);
            strcat(sb, tempResult);
        }
    }
    char res[KEY_SIZE + 1] = {0};
    strncpy(res, sb, NIBBLE_SIZE);
    strncat(res, sb + 2 * NIBBLE_SIZE, NIBBLE_SIZE);
    strncat(res, sb + NIBBLE_SIZE, NIBBLE_SIZE);
    strncat(res, sb + 3 * NIBBLE_SIZE, NIBBLE_SIZE);

    char roundOneResult[KEY_SIZE + 1] = {0};
    stringXOR(res, key1, roundOneResult);

    // Round 2 - Nibble Substitution -> Shift Row -> Add Key
    char roundTwoResult[KEY_SIZE + 1] = {0};
    char shiftRowResult2[KEY_SIZE + 1] = {0};
    char substituted2[NIBBLE_SIZE * 4 + 1] = {0};
    nibbleSubstitution(roundOneResult, SBOX, substituted2);
    shiftRow(substituted2, shiftRowResult2);
    stringXOR(shiftRowResult2, key2, roundTwoResult);

    strcpy(encryptedMsg, roundTwoResult);
}

int main() {
    char key[KEY_SIZE + 1] = {0}, msg[KEY_SIZE + 1] = {0};
    printf("Enter 16-bit key: ");
    scanf("%16s", key);

    printf("Enter 16-bit binary form message for encryption: ");
    scanf("%16s", msg);

    generateKeys(key);
    getKeys();

    printf("\nEncryption\n");
    char encryptedMsg[KEY_SIZE + 1] = {0};
    encrypt(msg, encryptedMsg);
    printf("Encrypted Message: %s\n", encryptedMsg);

    return 0;
}
