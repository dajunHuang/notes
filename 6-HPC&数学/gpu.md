| 型号 |  GTX 1650   | GTX 2080Ti |RTX 4090| A100 40GB |  H100 SXM    |
| :--: | :--: | :--: | :--: |:--:| :--: |
| **Compute Capability** | 7.5 (Volta)(Turing) | 7.5 (Volta)(Turing)(Ada) |8.9 (Ampere)(Ada)| 8.0 (Ampere) | 9.0 (Hopper) |
| GPU Memeory |      | 11GB GDDR6 |24GB GDDR6X| 40GB HBM2 | 80GB HBM3 memory |
| GPU Memory Bandwidth |      | 616GB/s | 1.008TB/s | 1.555TB/s |3.35TB/s|
| SM | | 68 |128| 108 |114|
| Max Thread Blocks / SM  | 32 | 32 |24| 32 |      32      |
|     Max Warps / SM      | 64 | 64 |48| 64 |      64      |
|     Max Threads / SM      | 2048 | 2048 |1536| 2048 |      2048      |
|     Max Threads / Thread Block      | 1024 | 1024 |1024| 1024 |      1024      |
|Shared Memory + L1 data cache||||192KB||
|L2 cache|||500MB|40MB||
| Max Shared Memory Size / SM | 64KB | 64KB |100KB| 164KB |228KB|
| Max Shared Memory / Thread Block | 64KB | 64KB |99KB| 163KB |227(194)KB|
| Max local memory / Thread | 512KB | 512KB |512KB| 512KB | 512KB |
| GPU processing clusters ||6|11|7|8|
| Max Thread Blocks / Thread Block Cluster |  |  ||  | 16 |

[Hopper GPU Architecture | NVIDIA](https://www.nvidia.com/en-us/data-center/technologies/hopper-architecture/)

[NVIDIA Hopper Architecture In-Depth | NVIDIA Technical Blog](https://developer.nvidia.com/blog/nvidia-hopper-architecture-in-depth/)

The full implementation of the GH100 GPU includes the following units:

- 8 GPCs, 72 TPCs (9 TPCs/GPC), 2 SMs/TPC, 144 SMs per full GPU
- 128 FP32 CUDA Cores per SM, 18432 FP32 CUDA Cores per full GPU
- 4 fourth-generation Tensor Cores per SM, 576 per full GPU
- 6 HBM3 or HBM2e stacks, 12 512-bit memory controllers
- 60 MB L2 cache
- Fourth-generation NVLink and PCIe Gen 5

<img src="https://cosmos-1251905798.cos.ap-beijing.myqcloud.com/2023-09-09_nvidia-gh100.png" alt="GH100 Full GPU with 144 SMs, Source: NVIDIA H100 WhitePaper" style="zoom:80%;" />



![NVIDIA GH100 GPU SM (Streaming Multiprocessor) block diagram](https://developer-blogs.nvidia.com/wp-content/uploads/2022/03/H100-Streaming-Multiprocessor-SM-625x869.png)

Fourth-generation Tensor Cores:
  - Up to 6x faster chip-to-chip compared to A100, including per-SM speedup, additional SM count, and higher clocks of H100.
  - On a per SM basis, the Tensor Cores deliver 2x the MMA (Matrix Multiply-Accumulate) computational rates of the A100 SM on equivalent data types, and 4x the rate of A100 using the new FP8 data type, compared to previous generation 16-bit floating point options.
  - Sparsity feature exploits fine-grained structured sparsity in deep learning networks, doubling the performance of standard Tensor Core operations
