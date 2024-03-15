[无重复字符的最长子串](https://leetcode.cn/problems/longest-substring-without-repeating-characters/)

```cpp
class Solution {

public:
    int lengthOfLongestSubstring(string s) {
        if(s.size() == 0)
            return 0;
        set<char> lookup;
        int max_len = 0;
        int left = 0;
        for(int i = 0; i < s.size(); ++ i) {
            // 第i个数要加进来，所以先删除重复的数字，直到下标left到i没有重复字符
            while(lookup.find(s[i]) != lookup.end()) 
                lookup.erase(s[left ++]);
            // 更新最长子串  
            max_len = max(max_len, i - left + 1);
            lookup.insert(s[i]);
        }
        return max_len;
    }
};
```

[反转链表](https://leetcode.cn/problems/reverse-linked-list/)

```cpp
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* tail = nullptr;
        ListNode* ptr = head;
        while(ptr != nullptr) {
            ListNode* temp = ptr->next;
            ptr->next = tail;
            tail = ptr;
            ptr = temp;
        }
        return tail;
    }
};
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        if (head == nullptr || head->next == nullptr) {
            return head;
        }
        // 这里的cur就是最后一个节点
        ListNode* cur = reverseList(head->next);
		// 这里请配合动画演示理解
		// 如果链表是 1->2->3->4->5，那么此时的cur就是5
		// 而head是4，head的下一个是5，下下一个是空
		// 所以head.next.next 就是5->4
        head->next->next = head;
        // 防止链表循环，需要将head.next设置为空
        head->next = nullptr;
        // 每层递归函数都返回cur，也就是最后一个节点
        return cur;
    }
};
```

[数组中的第K个最大元素](https://leetcode.cn/problems/kth-largest-element-in-an-array/)

```cpp
class Solution {
public:
    int findKthLargest(vector<int>& nums, int k) {
        priority_queue<int> q1;
        for(auto i : nums) 
            q1.push(i);
        for(int i = 0; i < k - 1; ++ i)
            q1.pop();
        return q1.top();
    }
};
```

[最大子数组和](https://leetcode.cn/problems/maximum-subarray/)

```cpp
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int max = -10000;
        int tmp = -10000;
        for(int n : nums) {
            tmp = (tmp + n) > n ? (tmp + n) : n;
            max = max > tmp ? max : tmp;
        }
        return max;
    }
};
```

