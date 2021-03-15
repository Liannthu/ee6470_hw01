# General description or introduction of the problem and your solution

The problem has two requirments. Firstly, it requires us to implement gaussian blur algorithm including file in and out functions in c++ code. We can verify the program by comparing the difference between provided image and generated image. Secondly, we need to transfer our code to systemC processes with the following requirements.
  * A Input process to parse image format and send pixels to next process
  * A Calculation process to receive pixels and do Gaussian blur. Then pass results to next process. In this part, Calculation process will send all necessary pixels for each Gaussian blur calculation.
  * A Output process to dump processed pixels as a BMP image.
  * Use SystemC datatype as possible for Calculation process.
  * Implement handshaking between Input and Calculation processes, and also Calculation and Output processes. 
We will discuss my solution in next section.

# Implementation details (data structure, flows and algorithms)

My implement is based on provided files in lab02-Sobel Filter with FIFO interface with some necessary modifications. 

![image](https://user-images.githubusercontent.com/76727373/111162933-976e8e80-85d7-11eb-85b1-dcc5908aa41c.png)

Main.cpp

In main.cpp file, most of the code remain unchanged. I only add three sc_fifo: i_r, i_g i_b as fifo between filter and testbench. 

![image](https://user-images.githubusercontent.com/76727373/111162963-a05f6000-85d7-11eb-958f-91639b0903d3.png)

Testbench.cpp

In Testbench.cpp, I alter do_sobel function by removing “total” variable and replacing it with three variables: o_r, o_g, o_b. Moreover, I apply min and max function in target_bitmap to avoid values over 255 or under 0. I made those modification so that my program can generate colored images rather than gray images. Besides, values form filter process will be added bias and be divided by factor to meet the requirements of gaussian blur. By using fifo my program can do the filtering in another systemC process. 


![image](https://user-images.githubusercontent.com/76727373/111162981-a5bcaa80-85d7-11eb-845d-16ad142d84ea.png)

SobelFilter.cpp

In SobelFilter.cpp, I changed the mask and the calculation process so that it can apply the mask on red, green, and blue separately, instead of making those colors gray in the previous process. 

# Additional features of your design and models

My program can generate colored gaussian blur.

# Experimental results (on different benchmarks and settings)

lena_std_short.bmp

<img width="441" alt="Screen Shot 2021-03-15 at 9 45 22 PM" src="https://user-images.githubusercontent.com/76727373/111163090-c127b580-85d7-11eb-841e-b1ddc4dc7741.png">
  
Image on the left is the original one and the one on the right is after the gaussian blur process. Although, the effect is not obvious, we can still see the difference in red circuit. The right one is vaguer than the left one

lena_sobel_short.bmp

<img width="441" alt="Screen Shot 2021-03-15 at 9 46 05 PM" src="https://user-images.githubusercontent.com/76727373/111163175-d997d000-85d7-11eb-883a-0c83f42c0c60.png">

  
I apply my program in image after sobel process. The difference is obvious. The one on the left in before and the one on the right is after.

# Discussions and conclusions.

This homework is quite hard, but it is without a doubet a good practice for us to learn systemC. Besides, we have to write our report in English and submit our homework to GitHub, which are not familiar to me. Anyway, thank you for providing us this challenge.
