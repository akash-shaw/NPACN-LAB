# Lab 06 Q&A: Error Detection

## 1. Error Detection vs Correction
**Q: Which methods can fix errors?**

*   **Parity:** Detects errors (Is the total count odd/even?). Cannot fix them. Cannot tell *which* bit flipped.
*   **CRC:** Detects errors (Is the remainder 0?). Very good at detecting "burst" errors (many bits flipped). Cannot fix them.
*   **Hamming Code:** **Correction** capable. The "Syndrome" calculation points to the exact index of the flipped bit, allowing us to flip it back.

## 2. Modulo-2 Arithmetic (CRC)
**Q: What is the difference between XOR and Subtraction?**

In CRC division, we use Modulo-2 arithmetic.
*   It is essentially binary subtraction **without borrowing**.
*   This is exactly equivalent to the **XOR** operation.
    *   `1 - 1 = 0` (XOR 0)
    *   `1 - 0 = 1` (XOR 1)
    *   `0 - 1 = 1` (XOR 1) -> No borrowing from neighbor!
    *   `0 - 0 = 0` (XOR 0)

## 3. Hamming Code Redundancy
**Q: How do we know how many redundancy bits (r) we need?**

We use the formula: `2^r >= m + r + 1`
*   `m` = data bits.
*   `r` = redundancy bits.
*   `+1` = to cover the case of "No Error".

Example: Data = 7 bits.
*   Try r=3: `2^3 = 8`. `7 + 3 + 1 = 11`. (8 >= 11? False).
*   Try r=4: `2^4 = 16`. `7 + 4 + 1 = 12`. (16 >= 12? True).
So, we need 4 check bits for 7 data bits.

## 4. Parity Limitations
**Q: Why is Parity Check considered weak?**

It fails if **two** bits flip.
*   Original: `1001` (Two 1s -> Even Parity).
*   Error (1 flip): `1000` (One 1 -> Odd Parity). **Detected!**
*   Error (2 flips): `0000` (Zero 1s -> Even Parity). **NOT Detected!**

Since noise on a wire often flips multiple adjacent bits, Simple Parity is rarely used alone for critical data.

## 5. Additional Viva Questions

### **Error Control Theory**
**Q: What is Hamming Distance?**
The number of bits that differ between two binary strings.
*   `1010` and `1001` -> Distance = 2 (last two bits differ).
*   **To Detect $d$ errors:** Minimum Hamming Distance must be $d + 1$.
*   **To Correct $c$ errors:** Minimum Hamming Distance must be $2c + 1$.

**Q: Why do we use CRC instead of Checksum for Ethernet/WiFi?**
*   **Checksum (IP/TCP):** Uses 1's Complement arithmetic. It is fast in software but weak against certain patterns (e.g., swapping bytes).
*   **CRC (Link Layer):** Uses Polynomial Division. It simulates complex bit manipulations that are very strong against **burst errors** (where noise corrupts a chunk of bits). It is easily implemented in hardware (shift registers).

### **Algorithm Details**
**Q: How does the Receiver know if the data is correct in Hamming Code?**
It calculates the "Syndrome".
1.  Recalculate parity bits based on received data.
2.  If Parity 1 is wrong, add 1 to Syndrome.
3.  If Parity 2 is wrong, add 2 to Syndrome.
4.  If Parity 4 is wrong, add 4 to Syndrome.
5.  **Final Value = 0:** No Error.
6.  **Final Value = X:** Error is at position X.

**Q: What is a "Generator Polynomial"?**
It is the divisor used in CRC. Even if you see it as `x^3 + x + 1`, the computer sees binary `1011`.
*   Both Sender and Receiver must agree on this specific pattern beforehand.
*   Standard ones exist (CRC-32, CRC-CCITT) to ensure compatibility.

