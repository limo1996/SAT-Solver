# Python Wrapper

This python scripts will call our c++ solver.

There are 2 arguments:
- ``mode``: there are 3 possible modes
    - ``integration-test-local`` -> runs the integration tests locally (sequentially)
    - ``parallel-test-local`` -> runs the parallel tests on the local machine with as many cores as available
    - ``parallel-test-euler`` -> will run the parallel test on euler
- ``nethz_username``: is only used with ``--mode parallel test_euler``, if not provided, it will be prompted for

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
## Euler Login
Before your euler account is ready to be used you need to accept some policy, in order to do that login manually via ssh:
```bash
ssh <netz-account-name>@euler.ethz.ch
```
Then accept whatever it asks you to accept.

Also note that the current ssh setup is meant to be run "within" ETH,
so please use a vpn client to tunnel to the eth network when you're not in the eth network.
You can find more information and how to set it up here: [https://sslvpn.ethz.ch/](https://sslvpn.ethz.ch/)

## Running on Euler
To run the parallel tests on euler run:
```bash
python main.py --mode parallel-test-euler
```
This will do the following steps:
- package everything that needs to be on euler into a ``tar`` file
- send the tar-file via ssh to euler -> you need to enter your netz password for that
- ssh to euler and enqueue the program in the queue with the necessary parameters

Once all that is done there should be a message on the command line that looks like this:
```
#########################################################
run_me_on_euler.sh is taking over
including necessary modules...
submitting compilation and run job to the batch system...
Job <52097985> is submitted to queue <normal.4h>.
job submitted, exiting
#########################################################
```

You can then ssh to euler manually with (once again you need to enter your netz password):
```bash
ssh -o PreferredAuthentications=password -o PubkeyAuthentication=no <your-netz-account>@euler.ethz.ch
```
then you should be on euler and you can watch your task completing (replace the number with the job-id from your command line output):
```bash
watch -n 5 bbjobs 52097985
```
- any cout output will be stored in: ``/cluster/home/<netz-id>/sat_solver/lsf.o<job-id>``
- per test file output will be stored depending on what is done in ``bsub_script.sh``...
