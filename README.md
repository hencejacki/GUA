# GUA

Use GitHub API to fetch user activity and display it in the terminal.

## Usage

1. Compile

```bash
make
```

2. Use

```bash
./bin/github-activity <username>
```

3. Example

```bash
./bin/github-activity hencejacki
Analyze result: 
- Create an repository named hencejacki/GUA.
- Create an repository named hencejacki/GUA.
- Push 1 commits to repo hencejacki/TTC.
- Push 1 commits to repo hencejacki/TTC.
- Push 1 commits to repo hencejacki/TTC.
- Push 4 commits to repo hencejacki/TTC.
- Push 1 commits to repo hencejacki/TTC.
- Create an repository named hencejacki/TTC.
- Create an repository named hencejacki/TTC.
```

## TODO

- [ ] Use cache
- [ ] Extend more events
- [ ] Optimize code logic
