set "cvVersion=4.1.0"

mkdir opencv-%cvVersion%
cd opencv-%cvVersion%
mkdir Installation
set count=1
set cwd="%cd%"

conda create -y -f -n OpenCV-%cvVersion%-py3 python=3.6 anaconda
conda install -y -n OpenCV-%cvVersion%-py3 numpy scipy matplotlib scikit-image scikit-learn ipython

git clone https://github.com/opencv/opencv.git
cd opencv
git checkout tags/%cvVersion%
cd ..

git clone https://github.com/opencv/opencv_contrib.git
cd opencv_contrib
git checkout tags/%cvVersion%
cd ..

conda activate OpenCV-%cvVersion%-py3

where python

set var1="C:\Users\chedelp\AppData\Local\Continuum\anaconda3\envs\OpenCV-4.1.0-py3\python.exe"

cd %var1%\..\..
set envsDir=%var1%\..\..
cd %cwd%
deactivate

cd opencv
mkdir build
cd build

set "envsDir2=%envsDir:\=/%"
cmake -G "Visual Studio 15 2017 Win64" -T host=x64 -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX="%cwd%/Installation" -DINSTALL_C_EXAMPLES=ON -DINSTALL_C_EXAMPLES=ON -DINSTALL_PYTHON_EXAMPLES=ON -DOPENCV_EXTRA_MODULES_PATH="%cwd%/opencv_contrib/modules" -DBUILD_opencv_saliency=OFF -DPYTHON3_EXECUTABLE="%envsDir2%/OpenCV-%cvVersion%-py3/python.exe" -DPYTHON3_INCLUDE_DIR="%envsDir2%/OpenCV-%cvVersion%-py3/include" -DPYTHON3_LIBRARY="%envsDir2%/OpenCV-%cvVersion%-py3/libs/python36.lib" -DPYTHON3_NUMPY_INCLUDE_DIRS="%envsDir2%/OpenCV-%cvVersion%-py3/Lib/site-packages/numpy/core/include" -DPYTHON3_PACKAGES_PATH="%envsDir2%/OpenCV-%cvVersion%-py3/Lib/site-packages" -DOPENCV_SKIP_PYTHON_LOADER=ON ..
cmake --build . --config Release --target INSTALL