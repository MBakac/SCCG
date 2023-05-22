# SCCG

This is the repository of our implementation of the SCCG algorithm explained in the paper *Shi et al. 2019. High efficiency referential genome compression algorithm*
(DOI: 10.1093/bioinformatics/bty934).

This project is part of the [Bioinformatics 1](https://www.fer.unizg.hr/en/course/enbio1) course at FER, Universitiy of Zagreb. 

Done by [Martin Bakač](https://www.martinbakac.from.hr/) and Marta Bonacin under the supervision of [izv. prof. dr. sc. Mirjana Domazet-Lošo](https://www.fer.unizg.hr/en/mirjana.domazet-loso).

## Idea

The main idea proposed by the paper is to exploit the inherent properties of genetic data in order to develop a compression algorithm with better compression ratios and a faster runtime.

## Requirements

 - `g++ >= 9.0`, might have to change std=c++2* in Makefile depending on version

 - `7z`, 7zip utility (`sudo apt install p7zip-full` for Ubuntu like distros)

## Running the program

```
# compiles the compression and decompression programs, respectively
make compile
make compileD

# compress with target file with reference file
SCCGC ./path_to_target_file.fa ./path_to_reference_file ./output_folder

# decompress with target file with reference file
SCCGD ./path_to_reference_file ./path_to_target_file.7z ./output_folder
```

