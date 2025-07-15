# FiniteElementMethod-Diploma

> **Note:** This project currently supports **Windows only**.  
> Developed and tested with **Visual Studio (MSVC)**, **Premake5**, and **vcpkg**.

---

## Setup

### Local development
1. Clone the repository:
  ```shell
  git clone --recursive https://github.com/Dar3cz3Q-University/FiniteElementMethod-Diploma
  cd FiniteElementMethod-Diploma
  ```
2. Install `vcpkg`
  ```shell
  cd External/vcpkg
  ./bootstrap-vcpkg.bat
  ```
3. Install dependencies (run from the root folder)
  ```shell
  ./External/vcpkg/vcpkg install
  ```
4. Run the setup script in [Scripts](/Scripts) directory
5. Open Visual Studio solution
