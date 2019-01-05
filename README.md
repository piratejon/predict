# predict
Satellite tracking, orbital prediction, open-source software

# Python
approach for turning this into something we can call from Python is:
1) separate predict.c into main and "libpredict" files.
2) make a python wrapper that calls into the minimally-modified "libpredict"
piece: <https://docs.python.org/3.7/extending/extending.html>
