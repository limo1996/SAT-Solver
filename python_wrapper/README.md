# Python Wrapper

This python scripts will call our c++ solver.

There are 3 modes:
- ``integration-test-local`` -> runs the integration tests locally (sequentially)
- ``parallel-test-local`` -> runs the parallel tests on the local machine with as many cores as available
- ``parallel-test-euler`` -> will run the parallel test on euler

Example usage:
```bash
python main.py --mode integration-test-local
```


## Setup
Use pip to setup the python environment.
Use a virtual environment in case you don't want to install the packages system wide.
On linux setting up the virtual environment would be:
```bash
virtualenv venv -p python2.7
```
Note: You have to use python 2.7, I was not able to set up the z3-solver package with python 3, sorry about that :-(

Install the pip requirements with:
```bash
pip install -r requirements.txt
```

To see if the environment is set up correctly run:
```bash
python z3_smoke_test.py
```



