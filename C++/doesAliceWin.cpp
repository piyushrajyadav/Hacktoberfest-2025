class Solution {
public:
    bool doesAliceWin(string s) {
        for (char ch : s) {
            if (isVowel(ch)) return true;  // Alice wins if any vowel exists
        }
        return false;  // no vowels -> Alice cannot move
    }

private:
    bool isVowel(char ch) {
        return ch=='a'||ch=='e'||ch=='i'||ch=='o'||ch=='u';
    }
};
