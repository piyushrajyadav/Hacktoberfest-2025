class Router {
public:
    // custom hasher for tuple<int,int,int>
    struct TripleHash {
        size_t operator()(const tuple<int,int,int>& key) const noexcept {
            size_t a = (size_t) get<0>(key);
            size_t b = (size_t) get<1>(key);
            size_t c = (size_t) get<2>(key);
            size_t res = a;
            res = res * 1000003u + b;
            res = res * 1000003u + c;
            return res;
        }
    };

    // Fenwick tree that supports push_back() while preserving invariants
    struct Fenwick {
        vector<int> bit;
        int n = 0;
        Fenwick() : n(0) {}

        // push a new element (increase size by 1) and set bit[pos] correctly
        void push_back() {
            int old_n = n;
            n++;
            if ((int)bit.size() < n + 1) bit.resize(n + 1, 0);

            int pos = n;
            int parent = pos - (pos & -pos);

            // sum over [1..old_n]
            int sum_pos = 0;
            for (int i = old_n; i > 0; i -= i & -i) sum_pos += bit[i];

            // sum over [1..parent]
            int sum_parent = 0;
            for (int i = parent; i > 0; i -= i & -i) sum_parent += bit[i];

            // set bit[pos] so that prefix sums remain correct
            bit[pos] = sum_pos - sum_parent;
        }

        void add(int idx, int val) {
            for (int i = idx; i <= n; i += i & -i) bit[i] += val;
        }
        int sum(int idx) {
            int s = 0;
            for (int i = idx; i > 0; i -= i & -i) s += bit[i];
            return s;
        }
        int rangeSum(int l, int r) {
            if (r < l) return 0;
            return sum(r) - sum(l - 1);
        }
    };

    int limit;
    deque<tuple<int,int,int,int>> q;  // (src, dst, ts, id)
    unordered_set<tuple<int,int,int>, TripleHash> seen; // duplicates among stored packets
    unordered_map<int, vector<int>> destTimestamps;     // dest -> timestamps (append-only)
    unordered_map<int, Fenwick> fenw;                   // dest -> fenwick over positions
    vector<int> idToPos; // id -> position (1-based) in destTimestamps[dest]
    vector<int> idToDest; // id -> destination
    int nextId = 0;

    Router(int memoryLimit) : limit(memoryLimit) {
        seen.reserve(1 << 17);
    }

    bool addPacket(int source, int destination, int timestamp) {
        tuple<int,int,int> key = {source, destination, timestamp};
        if (seen.count(key)) return false;

        int id = nextId++;
        q.emplace_back(source, destination, timestamp, id);
        seen.insert(key);

        auto &vec = destTimestamps[destination];
        vec.push_back(timestamp);
        int pos = (int)vec.size(); // 1-based position

        // ensure fenwick has room and set correct base before adding
        fenw[destination].push_back();
        fenw[destination].add(pos, 1);

        idToPos.push_back(pos);
        idToDest.push_back(destination);

        // enforce memory limit: evict oldest if needed
        if ((int)q.size() > limit) {
            auto [os, od, ot, oid] = q.front();
            q.pop_front();
            seen.erase({os, od, ot});
            int oldPos = idToPos[oid];
            fenw[od].add(oldPos, -1);
        }
        return true;
    }

    vector<int> forwardPacket() {
        if (q.empty()) return {};
        auto [s, d, t, id] = q.front();
        q.pop_front();
        seen.erase({s, d, t});
        int pos = idToPos[id];
        fenw[d].add(pos, -1);
        return {s, d, t};
    }

    int getCount(int destination, int startTime, int endTime) {
        auto it = destTimestamps.find(destination);
        if (it == destTimestamps.end()) return 0;
        auto &vec = it->second;
        auto lit = lower_bound(vec.begin(), vec.end(), startTime);
        auto rit = upper_bound(vec.begin(), vec.end(), endTime);
        if (lit == vec.end() || lit >= rit) return 0;
        int lpos = int(lit - vec.begin()) + 1;
        int rpos = int(rit - vec.begin());
        return fenw[destination].rangeSum(lpos, rpos);
    }
};
