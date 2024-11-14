# ZedBoard AXI 4K Boundary
該硬體 IP 的功能是處理 RGB64 格式、大小為 256x256 的影像，實現每個像素的 R 和 G 值交換。此 IP 通過 AXI-Full Master 與 DRAM 進行讀寫，通過 AXI-Lite Slave 與 CPU 溝通，並掛載於 AXI3 介面以連接 BUS。
該 IP 已在 Xilinx SDK 上成功驗證，並可通過 Petalinux 生成開機檔與操作系統，並在軟體端透過驅動程式控制與操作功能。
