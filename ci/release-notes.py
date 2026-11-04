#!/usr/bin/env python3
"""Release notes from git history: features and defect fixes since a baseline.

Commits follow Conventional Commits:  type(scope)!: description
  feat  -> Features            fix -> Defects fixed         anything else -> Other changes
  scope app/android/ui -> App,  fw/firmware -> Firmware,  ci/build -> CI
  "!" after the type, or "BREAKING CHANGE:" in the body, marks a breaking change.
  Issue references such as #42 are linked.
Commits without a prefix are listed under Other changes, so nothing is lost.

Baseline (first found): --baseline, the file release-baseline (first non-comment line), the latest v* tag
before the build's commit, else the whole history.

  ci/release-notes.py [--head REF] [--baseline REF] [--output FILE]   write the release notes (default: stdout)
  ci/release-notes.py --lint RANGE                                     warn about commits without a prefix
"""
import argparse
import os
import re
import subprocess
import sys
from datetime import datetime, timezone

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

CONVENTIONAL = re.compile(r'^(?P<type>[a-z]+)(?:\((?P<scope>[^)]+)\))?(?P<breaking>!)?: (?P<description>.+)$')
TYPES = {'feat', 'fix', 'perf', 'refactor', 'docs', 'test', 'build', 'ci', 'chore', 'style', 'revert'}
COMPONENTS = {'app': 'App', 'android': 'App', 'ui': 'App',
              'fw': 'Firmware', 'firmware': 'Firmware',
              'ci': 'CI', 'build': 'CI'}
ISSUE = re.compile(r'(?<![\w/])#(\d+)\b')
BOT_VERSION_COMMIT = re.compile(r'^Version: ')


def git(*args):
    return subprocess.run(['git', *args], cwd=ROOT, check=True, capture_output=True, text=True).stdout


def git_ok(*args):
    return subprocess.run(['git', *args], cwd=ROOT, capture_output=True, text=True).returncode == 0


def versions():
    out = subprocess.run(['bash', os.path.join('ci', 'version.sh')], cwd=ROOT, check=True,
                         capture_output=True, text=True).stdout
    return dict(line.split('=', 1) for line in out.split())


def find_baseline(head, explicit):
    if explicit:
        return explicit
    path = os.path.join(ROOT, 'release-baseline')
    if os.path.exists(path):
        for line in open(path, encoding='utf-8'):
            line = line.strip()
            if line and not line.startswith('#'):
                return line
    if git_ok('rev-parse', '--verify', '--quiet', head + '^'):
        tag = subprocess.run(['git', 'describe', '--tags', '--abbrev=0', '--match', 'v*', head + '^'],
                             cwd=ROOT, capture_output=True, text=True)
        if tag.returncode == 0:
            return tag.stdout.strip()
    return None


def commits(rev_range):
    log = git('log', '--no-merges', '--format=%H%x1f%h%x1f%an%x1f%as%x1f%s%x1f%b%x1e', rev_range)
    for record in log.split('\x1e'):
        if not record.strip():
            continue
        full, short, author, date, subject, body = record.strip('\n').split('\x1f')
        if author == 'github-actions[bot]' and BOT_VERSION_COMMIT.match(subject):
            continue  # build number bumps made by CI
        yield {'sha': full, 'short': short, 'author': author, 'date': date, 'subject': subject, 'body': body}


def classify(commit):
    match = CONVENTIONAL.match(commit['subject'])
    if not match or match['type'] not in TYPES:
        return 'other', 'General', commit['subject'], False, False
    scope = (match['scope'] or '').strip().lower()
    component = COMPONENTS.get(scope, scope.capitalize() if scope else 'General')
    breaking = bool(match['breaking']) or 'BREAKING CHANGE:' in commit['body']
    section = {'feat': 'features', 'fix': 'fixes'}.get(match['type'], 'other')
    return section, component, match['description'], breaking, True


def link_issues(text, repo_url):
    if not repo_url:
        return text
    return ISSUE.sub(lambda m: f'[#{m[1]}]({repo_url}/issues/{m[1]})', text)


def render(head, baseline):
    repo_url = None
    if os.environ.get('GITHUB_SERVER_URL') and os.environ.get('GITHUB_REPOSITORY'):
        repo_url = f"{os.environ['GITHUB_SERVER_URL']}/{os.environ['GITHUB_REPOSITORY']}"
    rev_range = f'{baseline}..{head}' if baseline else head
    v = versions()
    head_sha = git('rev-parse', head).strip()

    sections = {'features': [], 'fixes': [], 'other': []}
    unprefixed = 0
    for c in commits(rev_range):
        section, component, description, breaking, conventional = classify(c)
        unprefixed += not conventional
        refs = ' '.join(f'#{n}' for n in ISSUE.findall(c['body']) if f'#{n}' not in description)
        text = description + (f' ({refs})' if refs else '')
        sha = f"[{c['short']}]({repo_url}/commit/{c['sha']})" if repo_url else c['short']
        line = f"- **{component}:** {link_issues(text, repo_url)}" + (' **(breaking)**' if breaking else '') \
               + f" — {sha}, {c['author']}, {c['date']}"
        sections[section].append((component, line))

    out = [f"# Release notes — app {v['app_version']}, firmware {v['fw_version']}", '']
    out += ['| | |', '|---|---|',
            f"| App version | {v['app_version']} |",
            f"| Firmware version | {v['fw_version']} (paired with app {v['ui_version']}) |",
            f"| Commit | {head_sha} |",
            f"| Baseline | {baseline or 'start of history'} |",
            f"| Generated | {datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M UTC')} |", '']
    if v['ui_version'] != v['app_version']:
        out += [f"> **Note:** the firmware is paired with app {v['ui_version']}, not with this app version.", '']
    for key, title in (('features', 'Features'), ('fixes', 'Defects fixed'), ('other', 'Other changes')):
        out += [f'## {title}', '']
        items = sorted(sections[key], key=lambda item: item[0])  # stable: by component, then git order
        out += [line for _, line in items] if items else ['None.']
        out.append('')
    if unprefixed:
        out += [f'_{unprefixed} commit(s) without a type prefix are listed under Other changes._', '']
    return '\n'.join(out)


def lint(rev_range):
    bad = 0
    for c in commits(rev_range):
        if not classify(c)[4]:
            bad += 1
            print(f"::warning title=Commit without a type prefix::{c['short']} \"{c['subject']}\" - "
                  f"use e.g. 'feat(app): ...' or 'fix(fw): ...' so it is listed as a feature or a defect fix")
    print(f'{bad} commit(s) without a type prefix')


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--head', default='HEAD')
    parser.add_argument('--baseline')
    parser.add_argument('--output')
    parser.add_argument('--lint', metavar='RANGE')
    args = parser.parse_args()
    if args.lint:
        lint(args.lint)
        return
    text = render(args.head, find_baseline(args.head, args.baseline))
    if args.output:
        os.makedirs(os.path.dirname(os.path.abspath(args.output)), exist_ok=True)
        with open(args.output, 'w', encoding='utf-8', newline='\n') as f:
            f.write(text)
    else:
        sys.stdout.reconfigure(encoding='utf-8')
        print(text)


if __name__ == '__main__':
    main()
