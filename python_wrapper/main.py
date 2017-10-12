import click

from test import Tester


@click.command()
@click.option('--mode', type=click.Choice(['integration-test-local',
                                           'parallel-test-local',
                                           'parallel-test-euler']),
              prompt='please specify a mode ('
                     'integration-test-local|parallel-test-local|parallel-test-euler)')
def main(mode):
    if mode == 'integration-test-local':
        test_folder = 'integration_tests'
        tester = Tester(test_folder)
    elif mode == 'parallel-test-local':
        test_folder = 'parallel_tests'
        tester = Tester(test_folder)
    else:
        raise RuntimeError('Euler connection is not yet implemented')

    print('found ' + str(len(tester.files)) + ' test files in ' +
          tester.folder)
    tester.run_test()


if __name__ == '__main__':
    main()
