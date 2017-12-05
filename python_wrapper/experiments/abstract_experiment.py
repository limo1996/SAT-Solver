import json

from utils import get_experiment_id


class AbstractExperiment(object):
    def __init__(self):
        self.data = {}
        self.re_init_data()
        self.name = 'AbstractExperiment'
        self.figures_folder = '../../report/figures'

    def re_init_data(self):
        self.data = {'experiment_info': get_experiment_id()}

    def run_experiment(self):
        """Runs the experiment and stores the result in self.data"""
        raise RuntimeError('Abstract!')

    def results_to_json(self):
        """Writes the results of self.data to a json file"""
        json_string = json.dumps(self.data)
        with open('results/{}.json'.format(self.name), 'w') as f:
            f.truncate()
            f.write(json_string)

    def results_from_json(self):
        """Reads the results of an experiment from json file into self.data"""
        self.data = {}
        with open('results/{}.json'.format(self.name), 'r') as f:
            json_string = f.read()
        self.data = json.loads(json_string)

    def plot(self):
        """Plots the data"""
        raise RuntimeError('Abstract!')
