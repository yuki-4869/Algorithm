template<int SIGMA = 26, char BASE = 'a'>
struct AhoCorasick {
    struct Node {
        array<int, SIGMA> next;
        int fail;
        int terminal_count;
        int match_count;

        Node()
            : fail(0),
              terminal_count(0),
              match_count(0) {
            next.fill(-1);
        }
    };

    vector<Node> nodes;
    vector<int> terminal_node;
    vector<int> bfs_order;

    bool built;

    AhoCorasick()
        : nodes(1),
          built(false) {}

    int char_id(char c) const {
        int x = c - BASE;

        do { if (!(0 <= x && x < SIGMA)) std::abort(); } while (false);

        return x;
    }

    int add(const string& s) {
        do { if (!(!built)) std::abort(); } while (false);
        do { if (!(!s.empty())) std::abort(); } while (false);

        int v = 0;

        for (char c : s) {
            int x = char_id(c);

            if (nodes[v].next[x] == -1) {
                nodes[v].next[x] = nodes.size();
                nodes.emplace_back();
            }

            v = nodes[v].next[x];
        }

        int id = terminal_node.size();

        terminal_node.push_back(v);
        nodes[v].terminal_count++;

        return id;
    }

    void build() {
        do { if (!(!built)) std::abort(); } while (false);

        built = true;

        queue<int> que;

        bfs_order.clear();
        bfs_order.reserve(nodes.size());
        bfs_order.push_back(0);

        nodes[0].fail = 0;
        nodes[0].match_count =
            nodes[0].terminal_count;

        for (int c = 0; c < SIGMA; c++) {
            int to = nodes[0].next[c];

            if (to == -1) {
                nodes[0].next[c] = 0;
            } else {
                nodes[to].fail = 0;

                nodes[to].match_count =
                    nodes[to].terminal_count;

                que.push(to);
            }
        }

        while (!que.empty()) {
            int v = que.front();
            que.pop();

            bfs_order.push_back(v);

            for (int c = 0; c < SIGMA; c++) {
                int to = nodes[v].next[c];

                if (to == -1) {
                    nodes[v].next[c] =
                        nodes[nodes[v].fail].next[c];

                    continue;
                }

                nodes[to].fail =
                    nodes[nodes[v].fail].next[c];

                nodes[to].match_count =
                    nodes[to].terminal_count +
                    nodes[nodes[to].fail].match_count;

                que.push(to);
            }
        }
    }

    int transition(int state, char c) const {
        do { if (!(built)) std::abort(); } while (false);
        do { if (!(0 <= state && state < (int)nodes.size())) std::abort(); } while (false);

        return nodes[state].next[char_id(c)];
    }

    long long count_all(const string& s) const {
        do { if (!(built)) std::abort(); } while (false);

        long long result = 0;
        int state = 0;

        for (char c : s) {
            state = transition(state, c);

            result += nodes[state].match_count;
        }

        return result;
    }

    vector<long long> count_each(
        const string& s
    ) const {
        do { if (!(built)) std::abort(); } while (false);

        vector<long long> count(nodes.size(), 0);

        int state = 0;

        for (char c : s) {
            state = transition(state, c);

            count[state]++;
        }

        for (
            int i = (int)bfs_order.size() - 1;
            i >= 1;
            i--
        ) {
            int v = bfs_order[i];

            count[nodes[v].fail] += count[v];
        }

        vector<long long> result(
            terminal_node.size()
        );

        for (
            int id = 0;
            id < (int)terminal_node.size();
            id++
        ) {
            result[id] =
                count[terminal_node[id]];
        }

        return result;
    }

    bool contains(const string& s) const {
        return count_all(s) > 0;
    }

    int size() const {
        return nodes.size();
    }

    int pattern_count() const {
        return terminal_node.size();
    }
};

/*
細かい注意点
既定は英小文字a-z。全パターンをaddしてからbuildする。build後の追加は不可。
空文字列は登録不可。同じ文字列の重複登録・重なりのある出現も別々に数える。
入力文字はテンプレートの文字集合に含まれること。cnt[id]はaddが返したIDで参照する。

使える関数一覧
AhoCorasick<> ac;                              // a-z 用で初期化

int id = ac.add(s);                            // パターン s を追加しパターン番号を返す O(|s|)
ac.build();                                    // failure link と遷移を構築 O(ノード数 * SIGMA)

ac.count_all(text);                            // text 中の全パターンの出現数の合計 O(|text|)
                                                // 重複パターンも別々に数える
                                                // オーバーラップする出現も数える

auto cnt = ac.count_each(text);                // 各パターンの出現回数を求める
                                                // O(|text| + ノード数 + パターン数)

cnt[id];                                       // add() で返された id のパターンの出現回数

ac.contains(text);                             // 登録パターンが1つでも text に現れるなら true O(|text|)

int state = 0;
state = ac.transition(state,c);                // 文字 c を1文字読んだ後の状態 O(1)

ac.size();                                     // Trie / Automaton のノード数 O(1)
ac.pattern_count();                            // 登録したパターン数 O(1)

基本の関数
AhoCorasick<> ac;
int id = ac.add("aba");
ac.add("ba");
ac.build();
auto counts = ac.count_each("ababa");        // counts[id]==2
auto total = ac.count_all("ababa");          // 4
*/
