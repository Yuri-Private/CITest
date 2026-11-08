#!/usr/bin/env python3
"""Summarizes JUnit XML test results for a GitHub Actions run.

    python3 ci/test-report.py "<title>" <result files or glob patterns>...

Writes a table to the run page ($GITHUB_STEP_SUMMARY, or stdout) and an error annotation per failed test,
because job logs are visible only to signed-in users while the summary and annotations are public.
Exits 1 if a test failed or no result was found, so the step fails even when the test tool did not.
"""
import glob
import os
import sys
import xml.etree.ElementTree as ET


def escape(text):
    """Escapes a value for a workflow command (::error ...::value)."""
    return text.replace('%', '%25').replace('\r', '').replace('\n', '%0A')


def escape_property(text):
    """Escapes a property of a workflow command (::error title=...::)."""
    return escape(text).replace(':', '%3A').replace(',', '%2C')


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    title, patterns = sys.argv[1], sys.argv[2:]
    files = sorted({f for p in patterns for f in glob.glob(p, recursive=True)})

    passed, failed, skipped = 0, [], 0
    for path in files:
        for case in ET.parse(path).getroot().iter('testcase'):
            name = f"{case.get('classname', '')}.{case.get('name', '')}".strip('.')
            problem = case.find('failure')
            if problem is None:
                problem = case.find('error')
            if problem is not None:
                text = ((problem.get('message') or '') + '\n' + (problem.text or '')).strip()
                if len(text) < 20:  # CTest writes just "Failed"; the test's own output says what failed
                    text = (text + '\n' + (case.findtext('system-out') or '')).strip()
                failed.append((name, text))
            elif case.find('skipped') is not None:
                skipped += 1
            else:
                passed += 1

    total = passed + len(failed) + skipped
    lines = [f'### {title}', '']
    if total == 0:
        lines.append(f'No test results found ({", ".join(patterns)}).')
    else:
        status = 'passed' if not failed else 'FAILED'
        lines += ['| Tests | Passed | Failed | Skipped | Result |', '|---|---|---|---|---|',
                  f'| {total} | {passed} | {len(failed)} | {skipped} | {status} |']
        if failed:
            lines += ['', 'Failed tests:', ''] + [f'- `{name}`' for name, _ in failed]

    summary = os.environ.get('GITHUB_STEP_SUMMARY')
    if summary:
        with open(summary, 'a', encoding='utf-8') as out:
            out.write('\n'.join(lines) + '\n\n')
    else:
        print('\n'.join(lines) + '\n')

    for name, text in failed:
        print(f'::error title={escape_property(title + ": " + name)}::{escape(text[:2000])}')
    if total == 0:
        print(f'::error title={escape_property(title)}::No test results found')
    return 1 if failed or total == 0 else 0


if __name__ == '__main__':
    sys.exit(main())
