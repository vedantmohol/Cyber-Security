import java.util.Random;
import java.util.Scanner;

public class RSA {
    public static int gcd(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    public static int modInverse(int a, int m) {
        a = a % m;
        for (int x = 1; x < m; x++) {
            if ((a * x) % m == 1) {
                return x;
            }
        }
        return -1;
    }

    public static int powerMod(int base, int exponent, int mod) {
        int result = 1;
        base = base % mod;
        while (exponent > 0) {
            if (exponent % 2 == 1) {
                result = (result * base) % mod;
            }
            exponent = exponent >> 1;
            base = (base * base) % mod;
        }
        return result;
    }

    public static int generatePrime(int min, int max) {
        Random rand = new Random();
        int prime;
        boolean isPrime;
        do {
            prime = rand.nextInt(max - min + 1) + min;
            isPrime = true;
            if (prime <= 1) isPrime = false;
            for (int i = 2; i <= Math.sqrt(prime); i++) {
                if (prime % i == 0) {
                    isPrime = false;
                    break;
                }
            }
        } while (!isPrime);
        return prime;
    }

    public static void generateKeys(int[] keys) {
        int p = generatePrime(10, 100);
        int q = generatePrime(10, 100);

        int n = p * q;
        int phi = (p - 1) * (q - 1);

        int e = 3;
        while (gcd(e, phi) != 1) {
            e += 2;
        }

        int d = modInverse(e, phi);

        keys[0] = n;
        keys[1] = e;
        keys[2] = d;
    }

    public static int encrypt(int message, int e, int n) {
        return powerMod(message, e, n);
    }

    public static int decrypt(int encryptedMessage, int d, int n) {
        return powerMod(encryptedMessage, d, n);
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Random rand = new Random();

        int[] keys = new int[3];
        generateKeys(keys);

        int n = keys[0];
        int e = keys[1];
        int d = keys[2];

        System.out.println("Public Key: (e = " + e + ", n = " + n + ")");
        System.out.println("Private Key: (d = " + d + ", n = " + n + ")");

        System.out.print("Enter a message to encrypt (as an integer): ");
        int message = scanner.nextInt();

        int encryptedMessage = encrypt(message, e, n);
        System.out.println("Encrypted message: " + encryptedMessage);

        int decryptedMessage = decrypt(encryptedMessage, d, n);
        System.out.println("Decrypted message: " + decryptedMessage);

        scanner.close();
    }
}
