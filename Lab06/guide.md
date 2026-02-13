# Lab 06: Error Detection and Correction

This lab moves away from socket programming and focuses on the algorithms used at the Data Link Layer to ensure data integrity. These programs simulate the **Sender**, **Transmission Channel** (where you manually introduce errors), and **Receiver** within a single process.

## Exercise 1: Parity Check (`quo1.c`)

The simplest form of error detection. A single "parity bit" is added to the data so that the total number of 1s is either Even (Even Parity) or Odd (Odd Parity).

### Logic
1.  **Count 1s:** The function `count_ones(data)` counts the number of '1's in the input string.
2.  **Sender Calculation:**
    *   **Even Parity:** If count is Odd, Parity bit = 1 (making total even). Else 0.
    *   **Odd Parity:** If count is Even, Parity bit = 1 (making total odd). Else 0.
3.  **Receiver Verification:**
    *   Recalculate the number of 1s (including the received parity bit).
    *   If the result matches the expected parity (e.g., Even Parity -> Total 1s is Even), data is accepted. Otherwise, rejected.

### Limitation
Parity check can detect **single-bit errors** but cannot detect if two bits flip (as the count would remain even/odd). It cannot correct errors.

---

## Exercise 2: Cyclic Redundancy Check (CRC) (`quo2.c`)

A more robust error detection method using polynomial division (Modulo-2 Arithmetic).

### Key Concepts
*   **Generator Polynomial:** A fixed binary string (divisor) agreed upon by both sender and receiver.
    *   CRC-12: `1100000001111`
    *   CRC-16: `11000000000000101`
    *   CRC-CCITT: `10001000000100001`
*   **Padding:** The data is padded with `N-1` zeros, where `N` is the length of the generator.

### The Algorithm (Sender)
1.  **XOR Division:** The code performs binary long division using XOR instead of subtraction.
2.  **Remainder:** The remainder of this division is the **CRC Checksum**.
3.  **Transmission:** Sender sends `Data + Checksum`.

### The Algorithm (Receiver)
1.  **Verify:** The receiver divides the received message (`Data + Checksum`) by the same Generator.
2.  **Result:** If the remainder is **0**, the data is valid. Any non-zero remainder indicates an error.

---

## Exercise 3: Hamming Code (`quo3.c`)

Unlike Parity and CRC which only *detect* errors, Hamming Code can **correct** single-bit errors.

### 1. Redundancy Bits (r)
We need enough parity bits to cover all data bits plus the parity bits themselves.
Formula: $2^r \ge m + r + 1$
*   `m`: Number of data bits.
*   `r`: Number of redundancy bits.

### 2. Positioning
*   **Parity Bits:** Placed at positions that are powers of 2 (1, 2, 4, 8...).
*   **Data Bits:** Placed at all other positions (3, 5, 6, 7...).

### 3. Calculation (Sender)
Each parity bit covers specific positions.
*   **P1 (Pos 1):** Checks bits 1, 3, 5, 7, 9... (Skip 1, Check 1)
*   **P2 (Pos 2):** Checks bits 2, 3, 6, 7, 10... (Skip 2, Check 2)
*   **P4 (Pos 4):** Checks bits 4, 5, 6, 7, 12... (Skip 4, Check 4)

The code calculates the value (0 or 1) for P1, P2, etc., to satisfy Even Parity for their respective groups.

### 4. Syndrome (Receiver)
The receiver recalculates the parity bits based on the received data.
*   If P1 is wrong, add 1 to `error_pos`.
*   If P2 is wrong, add 2 to `error_pos`.
*   If P4 is wrong, add 4 to `error_pos`.

**Result:** The final value of `error_pos` tells you strictly **which bit location has the error**. If it's 0, no error. If it's 7, bit 7 is flipped—so we flip it back to correct it.
