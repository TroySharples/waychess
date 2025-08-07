import argparse
import pandas as pd
import sys

def filter_puzzles(df, args):
    if args.theme:
        df = df[df['Themes'].str.contains(args.theme, case=False, na=False)]

    if args.min_rating is not None:
        df = df[df['Rating'] >= args.min_rating]

    if args.max_rating is not None:
        df = df[df['Rating'] <= args.max_rating]

    if args.contains_opening:
        df = df[df['OpeningTags'].str.contains(args.contains_opening, case=False, na=False)]

    return df

def sort_puzzles(df, args):
    sort_map = {
        "highest-rated": ("Rating", False),
        "lowest-rated": ("Rating", True),
        "most-popular": ("Popularity", False),
        "least-popular": ("Popularity", True),
    }

    if args.sort_by:
        col, asc = sort_map[args.sort_by]
        df = df.sort_values(by=col, ascending=asc)

    return df

def main():
    parser = argparse.ArgumentParser(description="Filter and sort a chess puzzle CSV.")
    parser.add_argument("csv_file", help="Path to a Lichess CSV puzzle file")

    parser.add_argument("--theme", help="Filter by theme (e.g. 'mate')")
    parser.add_argument("--contains-opening", help="Filter by opening (e.g. 'Sicilian')")
    parser.add_argument("--min-rating", type=int, help="Minimum rating")
    parser.add_argument("--max-rating", type=int, help="Maximum rating")

    parser.add_argument(
        "--sort-by",
        choices=["highest-rated", "lowest-rated", "most-popular", "least-popular"],
        help="Sort puzzles by one of these options"
    )

    args = parser.parse_args()

    df = pd.read_csv(args.csv_file)
    df = filter_puzzles(df, args)
    df = sort_puzzles(df, args)

    # Avoids ugly broken-pipe trace when used in conjunction with tools like head.
    try:
        df.to_csv(sys.stdout, index=False)
    except BrokenPipeError:
        sys.exit(0)

if __name__ == "__main__":
    main()