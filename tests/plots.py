import matplotlib.pyplot as plt

n = int(input())

with open("treap_data.txt", "r") as f:
    treap_avg_insert_time = list(map(int, f.readline().strip().split()))
    treap_avg_delete_time = list(map(int, f.readline().strip().split()))
    treap_avg_find_time = list(map(int, f.readline().strip().split()))

with open("splay_data.txt", "r") as f:
    splay_avg_insert_time = list(map(int, f.readline().strip().split()))
    splay_avg_delete_time = list(map(int, f.readline().strip().split()))
    splay_avg_find_time = list(map(int, f.readline().strip().split()))

with open("avl_data.txt", "r") as f:
    avl_avg_insert_time = list(map(int, f.readline().strip().split()))
    avl_avg_delete_time = list(map(int, f.readline().strip().split()))
    avl_avg_find_time = list(map(int, f.readline().strip().split()))

with open("rb_data.txt", "r") as f:
    rb_avg_insert_time = list(map(int, f.readline().strip().split()))
    rb_avg_delete_time = list(map(int, f.readline().strip().split()))
    rb_avg_find_time = list(map(int, f.readline().strip().split()))

with open("naive_data.txt", "r") as f:
    naive_avg_insert_time = list(map(int, f.readline().strip().split()))
    naive_avg_delete_time = list(map(int, f.readline().strip().split()))
    naive_avg_find_time = list(map(int, f.readline().strip().split()))

x = [i for i in range(n)]

plt.plot(x, treap_avg_insert_time)
plt.plot(x, avl_avg_insert_time)
plt.plot(x, rb_avg_insert_time)
plt.plot(x, naive_avg_insert_time)

plt.savefig("img.png")
