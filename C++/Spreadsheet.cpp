class Spreadsheet {
private:
    vector<vector<int>> grid; // rows x 26
    int rows;

    // Convert cell string like "A1" -> (row, col)
    pair<int,int> parseCell(const string &cell) {
        char colChar = cell[0];
        int col = colChar - 'A';
        int row = stoi(cell.substr(1)) - 1; // convert to 0-based
        return {row, col};
    }

    // Get value of either cell or number
    int getTokenValue(const string &token) {
        if (isdigit(token[0])) {
            return stoi(token);
        }
        auto [r, c] = parseCell(token);
        return grid[r][c];
    }

public:
    Spreadsheet(int rows) {
        this->rows = rows;
        grid.assign(rows, vector<int>(26, 0));
    }

    void setCell(string cell, int value) {
        auto [r, c] = parseCell(cell);
        grid[r][c] = value;
    }

    void resetCell(string cell) {
        auto [r, c] = parseCell(cell);
        grid[r][c] = 0;
    }

    int getValue(string formula) {
        // formula format: =X+Y
        string expr = formula.substr(1); // remove '='
        int plusPos = expr.find('+');
        string left = expr.substr(0, plusPos);
        string right = expr.substr(plusPos + 1);

        int val1 = getTokenValue(left);
        int val2 = getTokenValue(right);

        return val1 + val2;
    }
};

/**
 * Your Spreadsheet object will be instantiated and called as such:
 * Spreadsheet* obj = new Spreadsheet(rows);
 * obj->setCell(cell,value);
 * obj->resetCell(cell);
 * int param_3 = obj->getValue(formula);
 */
